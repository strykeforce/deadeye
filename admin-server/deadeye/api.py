from flask_socketio import SocketIO
from threading import Lock
import json
from networktables import NetworkTables
from .models import Unit


class Api:
    def __init__(self, app):
        self.app = app
        self.socketio = SocketIO(app=app, logger=app.logger)
        self.thread = None  # background thread for client model refresh
        self.refresh = False  # background thread broadcasts changes when True
        self.thread_lock = Lock()
        self.nt_connecting = False
        self.nt_connected = False
        self.socketio.on_event("message", self.handle_message)
        self.socketio.on_event("camera_control", self.handle_camera_control_event)
        self.socketio.on_event("camera_config", self.handle_camera_config_event)
        self.socketio.on_event("connect", self.handle_connect)

    def handle_message(self, message):
        self.app.logger.debug("received message: " + str(message))

    def handle_camera_control_event(self, j):
        unit = Unit.units[j["unit"]]
        camera = unit.cameras[str(j["inum"])]
        enabled = j["enabled"]
        camera.enable(enabled)
        self.app.logger.debug(
            "unit: %s, camera: %s, enabled: %s", unit.id, camera.id, enabled
        )

    def handle_camera_config_event(self, j):
        unit = Unit.units[j["unit"]]
        camera = unit.cameras[str(j["inum"])]
        config = j["config"]
        camera.set_config(config)
        self.app.logger.debug(
            "unit: %s, camera: %s, enabled: %s", unit.id, camera.id, camera.config
        )

    def handle_connect(self):
        self.app.logger.debug("client connected")
        if not self.nt_connected and not self.nt_connecting:
            self.nt_connecting = True
            self.app.logger.debug("connecting to NetworkTables...")
            NetworkTables.initialize(server="127.0.0.1")
            NetworkTables.addConnectionListener(
                self.nt_connection_listener, immediateNotify=True
            )

        with self.thread_lock:
            if self.thread is None:
                self.thread = self.socketio.start_background_task(
                    self.background_thread, self.app
                )
                self.app.logger.debug("started model refresh thread")

        self.refresh = self.nt_connected

    def nt_connection_listener(self, is_connected, info):
        self.nt_connected = is_connected
        self.nt_connecting = not is_connected
        Unit.init(self)
        self.app.logger.debug("Connected = %s, info = %s", is_connected, info)

    def background_thread(self, app):
        while True:
            if self.refresh:
                self.app.logger.debug("model refresh available")
                self.socketio.emit(
                    "refresh", json.dumps(Unit.units, default=lambda o: o.__dict__)
                )
                self.refresh = False
            self.socketio.sleep(0.250)
