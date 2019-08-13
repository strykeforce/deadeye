import os
import logging
from flask import Flask
from flask_socketio import SocketIO

logging.basicConfig(level=logging.DEBUG)

app = Flask(
    __name__, static_folder="../static/build/static", template_folder="../static/build"
)
app.config["SECRET_KEY"] = os.environ.get("SECRET_KEY") or "178f0ddde0198c9ba3cb6271"

socketio = SocketIO(app=app, logger=app.logger)
