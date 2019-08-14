# pylint: disable=global-statement
from threading import Lock
import simplejson as json
from networktables import NetworkTables

import models

thread = None
thread_lock = Lock()

nt_connecting = False
nt_connected = False

socketio = None
app = None


def init(s, a):
    global socketio, app
    socketio, app = s, a
    socketio.on_event("message", handle_message)
    socketio.on_event("camera_control", handle_camera_control_event)
    socketio.on_event("camera_config", handle_camera_config_event)
    socketio.on_event("connect", handle_connect)


def handle_message(message):
    app.logger.debug("received message: " + str(message))


def handle_camera_control_event(j):
    unit = models.Unit.units[j["unit"]]
    camera = unit.cameras[str(j["inum"])]
    enabled = j["enabled"]
    camera.enable(enabled)
    app.logger.debug("unit: %s, camera: %s, enabled: %s", unit.id, camera.id, enabled)


def handle_camera_config_event(j):
    unit = models.Unit.units[j["unit"]]
    camera = unit.cameras[str(j["inum"])]
    config = j["config"]
    camera.set_config(config)
    app.logger.debug(
        "unit: %s, camera: %s, enabled: %s", unit.id, camera.id, camera.config
    )


def handle_connect():
    app.logger.debug("client connected")
    global nt_connected
    global nt_connecting
    if not nt_connected and not nt_connecting:
        nt_connecting = True
        app.logger.debug("connecting to NetworkTables...")
        NetworkTables.initialize(server="127.0.0.1")
        NetworkTables.addConnectionListener(
            nt_connection_listener, immediateNotify=True
        )

    global thread
    with thread_lock:
        if thread is None:
            thread = socketio.start_background_task(background_thread, app)
            app.logger.debug("started model update thread")

    models.update_available = nt_connected


def nt_connection_listener(is_connected, info):
    global nt_connected
    global nt_connecting
    nt_connected = is_connected
    nt_connecting = not is_connected
    models.Unit.init()
    app.logger.debug("Connected = %s, info = %s", is_connected, info)


def background_thread(app):
    while True:
        if models.update_available:
            app.logger.debug("refresh available")
            socketio.emit(
                "refresh", json.dumps(models.Unit.units, cls=models.UnitEncoder)
            )
            models.update_available = False
        socketio.sleep(0.250)
