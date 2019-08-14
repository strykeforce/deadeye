import os
from flask import Flask, render_template
import simplejson as json
from . import models


def create_app():
    app = Flask(
        __name__,
        static_folder="../admin-web/build/static",
        template_folder="../admin-web/build",
    )
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
        return json.dumps(models.Unit.units, cls=models.UnitEncoder)

    return app
