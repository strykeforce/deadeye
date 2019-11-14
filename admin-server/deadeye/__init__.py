import os
import json
from flask import Flask, render_template
from .models import Unit


def create_app():
    app = Flask(__name__, static_folder="assets/static", template_folder="assets")
    app.config.from_mapping(SECRET_KEY="dev")
    app.config.from_envvar("DEADEYE_SETTINGS", silent=True)

    # pylint: disable=unused-variable
    @app.route("/")
    def index():
        return render_template("index.html")

    @app.route("/manifest.json")
    def manifest():
        return render_template("manifest.json")

    @app.route("/json")
    def json_dump():
        return json.dumps(Unit.units, default=lambda o: o.__dict__)

    return app
