"""
This script is used to start the admin server app.
"""
import os
import logging
from deadeye import create_app
from deadeye.api import Api


def main():
    logging.basicConfig(level=logging.WARN)
    logging.getLogger("deadeye").setLevel(
        logging.INFO
        if "FLASK_ENV" in os.environ and os.environ["FLASK_ENV"] == "production"
        else logging.DEBUG
    )

    PORT = (
        os.environ["DEADEYE_ADMIN_PORT"] if "DEADEYE_ADMIN_PORT" in os.environ else 5000
    )

    APP = create_app()
    API = Api(APP)
    API.socketio.run(APP, host="0.0.0.0", port=PORT)