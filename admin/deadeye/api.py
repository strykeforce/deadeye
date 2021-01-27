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
        self.load_units = False
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
        self.app.logger.debug("api: camera: %s, enabled: %s", camera.id, enabled)

    def handle_light_control_event(self, message):
        unit = Unit.units[message["unit"]]
        camera = unit.cameras[str(message["inum"])]
        enabled = message["enabled"]
        camera.light.enable(enabled)
        self.app.logger.debug("api: camera: %s, light enabled: %s", camera.id, enabled)

    def handle_capture_config_event(self, message):
        unit = Unit.units[message["unit"]]
        camera = unit.cameras[str(message["inum"])]
        capture = message["capture"]
        camera.set_capture(capture)
        self.app.logger.debug("api: camera: %s, capture: %s", camera.id, camera.capture)

    def handle_pipeline_config_event(self, message):
        unit = Unit.units[message["unit"]]
        camera = unit.cameras[str(message["inum"])]
        pipeline = message["pipeline"]
        camera.set_pipeline(pipeline)
        self.app.logger.debug(
            "api: camera: %s, pipeline: %s", camera.id, camera.pipeline
        )

    def handle_stream_config_event(self, message):
        unit = Unit.units[message["unit"]]
        camera = unit.cameras[str(message["inum"])]
        stream = message["stream"]
        camera.set_stream(stream)
        self.app.logger.debug("api: camera: %s, stream: %s", camera.id, camera.stream)

    def handle_image_upload_event(self, message):
        unit = Unit.units[message["unit"]]
        camera = unit.cameras[str(message["inum"])]
        filename = message["image"]
        path = os.path.join(os.environ["DEADEYE_UPLOAD_DIR"], filename)
        capture = camera.capture
        capture["config"]["image"] = path
        self.app.logger.debug(f"CAPTURE: {capture}")
        camera.set_capture(capture)
        self.app.logger.debug("api: camera: %s, upload: %s", camera.id, path)

    def handle_link_refresh_event(self, message):
        self.app.logger.debug("Link refresh event")
        self.refresh_link = True

    def handle_link_config_event(self, message):
        link = message["link"]
        self.link.set_entries(link)
        self.app.logger.debug("link: %s", link)

    def handle_connect(self):
        self.app.logger.debug("web client connected")

        def connection_callback(connected):
            self.running = connected

            if not connected:
                self.app.logger.error("api callback: connection failed")
                return

            self.load_units = True

        self.nt.connect(connection_callback)

        with self.thread_lock:
            if self.thread is None:
                self.thread = self.socketio.start_background_task(
                    self.background_thread, self.app
                )
                self.app.logger.debug("started model refresh thread")

    def background_thread(self, app):
        while self.running:
            if self.load_units:
                self.load_units = False
                self.socketio.sleep(0.250)  # wait for NT to populate
                self.app.logger.info("initializing Deadeye Units")
                with self.app.app_context():
                    Unit.init(self)
                    self.link = Link(self)
                    self.refresh_units = True
                    self.refresh_link = True

            if self.refresh_units:
                self.app.logger.debug("client units refresh")
                self.socketio.emit(
                    "refresh", json.dumps(Unit.units, default=lambda o: o.__dict__)
                )
                self.refresh_units = False
            if self.refresh_link:
                self.app.logger.debug("client link refresh")
                self.socketio.emit(
                    "link", json.dumps(self.link.entries, default=lambda o: o.__dict__)
                )
                self.refresh_link = False

            self.socketio.sleep(0.250)
        self.app.logger.warn("Exit model refresh thread")
