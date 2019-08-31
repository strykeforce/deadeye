"""
This script is used to start the admin server app.
"""
import os
import logging
from deadeye import create_app
from deadeye.api import Api

logging.basicConfig(level=logging.WARN)
logging.getLogger("deadeye").setLevel(
    logging.INFO if os.environ["FLASK_ENV"] == "production" else logging.DEBUG
)

APP = create_app()
API = Api(APP)
API.socketio.run(APP)
