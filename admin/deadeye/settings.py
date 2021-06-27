import logging
import os

from dotenv import load_dotenv

load_dotenv()
DEADEYE_ADMIN_PORT = os.getenv("DEADEYE_ADMIN_PORT")
DEADEYE_NT_WAIT_MS = int(os.getenv("DEADEYE_NT_WAIT_MS"))
DEADEYE_UPLOAD_DIR = os.getenv("DEADEYE_UPLOAD_DIR")
DEADEYE_LOGGING = (
    logging.INFO if os.environ["FLASK_ENV"] == "production" else logging.DEBUG
)
