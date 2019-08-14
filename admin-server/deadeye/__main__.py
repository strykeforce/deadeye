"""
This script is used to start the admin server app.
"""
import os
import logging
from flask_socketio import SocketIO
from deadeye import create_app, api

LOG_LEVEL = logging.INFO if os.environ["FLASK_ENV"] == "production" else logging.DEBUG
logging.basicConfig(level=LOG_LEVEL)

APP = create_app()
SOCKETIO = SocketIO(app=APP, logger=APP.logger)
api.init(SOCKETIO, APP)
SOCKETIO.run(APP)
