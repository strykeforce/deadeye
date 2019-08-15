"""
This script is used to start the admin server app.
"""
import os
import logging
from deadeye import create_app
from deadeye.api import Api

LOG_LEVEL = logging.INFO if os.environ["FLASK_ENV"] == "production" else logging.DEBUG
logging.basicConfig(level=LOG_LEVEL)

APP = create_app()
API = Api(APP)
API.socketio.run(APP)
