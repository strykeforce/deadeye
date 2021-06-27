"""
This script is used to start the admin server app.
"""
import logging

from deadeye.app import create_app
from deadeye.api import Api
from deadeye.settings import DEADEYE_ADMIN_PORT, DEADEYE_LOGGING

logging.basicConfig(level=logging.WARN)


def main():
    logging.getLogger("deadeye").setLevel(DEADEYE_LOGGING)

    APP = create_app()
    API = Api(APP)
    API.socketio.run(APP, host="0.0.0.0", port=DEADEYE_ADMIN_PORT)


if __name__ == "__main__":
    main()
