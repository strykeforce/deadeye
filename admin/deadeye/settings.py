import logging
import os

from dotenv import load_dotenv

load_dotenv()
DEADEYE_NT_SERVER = os.environ["DEADEYE_NT_SERVER"]
DEADEYE_ADMIN_PORT = os.environ["DEADEYE_ADMIN_PORT"]
DEADEYE_NT_WAIT_MS = int(os.environ["DEADEYE_NT_WAIT_MS"])
DEADEYE_UPLOAD_DIR = os.environ["DEADEYE_UPLOAD_DIR"]
DEADEYE_LOGGING = (
    logging.INFO if os.environ["FLASK_ENV"] == "production" else logging.DEBUG
)
