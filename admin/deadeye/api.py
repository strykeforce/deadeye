from flask_socketio import SocketIO
from threading import Lock
import json
import os
import sys

from .models import Unit, Link
from .nt_connection import NetworkTablesConnection


class Api:
    def __init__(self, app):
        self.app = app
        self.socketio = SocketIO(app=app, logger=False, engineio_logger=False)
        self.thread = None  # background thread for client model refresh
        self.refresh_units = False  # background thread broadcasts changes when True
        self.refresh_link = False
        self.thread_lock = Lock()
        self.nt = NetworkTablesConnection(self)

        self.socketio.on_event("connect", self.handle_connect)
        self.socketio.on_event("message", self.handle_message)
        self.socketio.on_event("camera_control", self.handle_camera_control_event)
        self.socketio.on_event("light_control", self.handle_light_control_event)
        self.socketio.on_event("capture_config", self.handle_capture_config_event)
        self.socketio.on_event("pipeline_config", self.handle_pipeline_config_event)
        self.socketio.on_event("stream_config", self.handle_stream_config_event)
        self.socketio.on_event("image_upload", self.handle_image_upload_event)

        self.link = None
        self.socketio.on_event("link_config", self.handle_link_config_event)
        self.socketio.on_event("link_refresh", self.handle_link_refresh_event)
        self.running = True

    def handle_message(self, message):
        self.refresh_units = True
        self.app.logger.info("received message: " + str(message))

    def handle_camera_control_event(self, message):
        unit = Unit.units[message["unit"]]
        camera = unit.cameras[str(message["inum"])]
        enabled = message["enabled"]
        camera.enable(enabled)
        self.app.logger.debug(
            "unit: %s, camera: %s, enabled: %s", unit.id, camera.id, enabled
        )

    def handle_light_control_event(self, message):
        unit = Unit.units[message["unit"]]
        camera = unit.cameras[str(message["inum"])]
        enabled = message["enabled"]
        camera.light.enable(enabled)
        self.app.logger.debug(
            "unit: %s, camera: %s, light enabled: %s", unit.id, camera.id, enabled
        )

    def handle_capture_config_event(self, message):
        unit = Unit.units[message["unit"]]
        camera = unit.cameras[str(message["inum"])]
        capture = message["capture"]
        camera.set_capture(capture)
        self.app.logger.debug(
            "unit: %s, camera: %s, capture: %s", unit.id, camera.id, camera.pipeline
        )

    def handle_pipeline_config_event(self, message):
        unit = Unit.units[message["unit"]]
        camera = unit.cameras[str(message["inum"])]
        pipeline = message["pipeline"]
        camera.set_pipeline(pipeline)
        self.app.logger.debug(
            "unit: %s, camera: %s, pipeline: %s", unit.id, camera.id, camera.pipeline
        )

    def handle_stream_config_event(self, message):
        unit = Unit.units[message["unit"]]
        camera = unit.cameras[str(message["inum"])]
        stream = message["stream"]
        camera.set_stream(stream)
        self.app.logger.debug(
            "unit: %s, camera: %s, stream: %s", unit.id, camera.id, camera.stream
        )

    def handle_image_upload_event(self, message):
        unit = Unit.units[message["unit"]]
        camera = unit.cameras[str(message["inum"])]
        filename = message["image"]
        path = os.path.join(os.environ["DEADEYE_UPLOAD_DIR"], filename)
        capture = camera.capture
        capture["config"]["image"] = path
        self.app.logger.debug(f"CAPTURE: {capture}")
        camera.set_capture(capture)
        self.app.logger.debug(
            "unit: %s, camera: %s, stream: %s", unit.id, camera.id, camera.stream
        )

    def handle_link_refresh_event(self, message):
        self.app.logger.debug("Link refresh event")
        self.refresh_link = True

    def handle_link_config_event(self, message):
        link = message["link"]
        self.link.set_entries(link)
        self.app.logger.debug("link: %s", link)

    def handle_connect(self):
        self.app.logger.debug("web client connected")

        def connection_callback(is_connected):
            self.running = is_connected

            if not is_connected:
                self.app.logger.error("API callback: connection failed")
                return

            self.app.logger.info("initializing Deadeye Units")
            with self.app.app_context():
                Unit.init(self)
                self.link = Link(self)

        self.nt.connect(connection_callback)

        with self.thread_lock:
            if self.thread is None:
                self.thread = self.socketio.start_background_task(
                    self.background_thread, self.app
                )
                self.app.logger.debug("started model refresh thread")

        self.refresh_units = self.nt.connected

    def background_thread(self, app):
        while self.running:
            if self.refresh_units:
                self.app.logger.debug("units refresh available")
                self.socketio.emit(
                    "refresh", json.dumps(Unit.units, default=lambda o: o.__dict__)
                )
                self.refresh_units = False
            if self.refresh_link:
                self.app.logger.debug("link refresh available")
                self.socketio.emit(
                    "link", json.dumps(self.link.entries, default=lambda o: o.__dict__)
                )
                self.refresh_link = False

            self.socketio.sleep(0.250)
        self.app.logger.warn("Exit model refresh thread")
