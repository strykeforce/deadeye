from flask import render_template
import simplejson as json
from app import app
import models

@app.route("/")
def index():
    return render_template("index.html")


@app.route("/manifest.json")
def manifest():
    return render_template("manifest.json")

@app.route("/json")
def json_dump():
    return json.dumps(models.Unit.units, cls=models.UnitEncoder)
