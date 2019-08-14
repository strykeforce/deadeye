from app import app, socketio
from api import *
from models import *
from views import *

if __name__ == "__main__":
    socketio.run(app)
