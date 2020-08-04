import os
import json
from flask import Flask, render_template, make_response, request, jsonify
from werkzeug.utils import secure_filename
from .models import Unit


def create_app():
    app = Flask(__name__, static_folder="assets/static", template_folder="assets")
    app.config["MAX_CONTENT_LENGTH"] = 8 * 1024 * 1024
    app.config.from_mapping(SECRET_KEY="dev")

    # pylint: disable=unused-variable
    @app.route("/")
    def index():
        return render_template("index.html")

    @app.route("/manifest.json")
    def manifest():
        return render_template("manifest.json")

    @app.route("/json")
    def json_dump():
        r = make_response(json.dumps(Unit.units, default=lambda o: o.__dict__))
        r.mimetype = "application/json"
        return r

    def allowed_file(filename):
        return "." in filename and filename.rsplit(".", 1)[1].lower() in {
            "png",
            "jpg",
            "jpeg",
        }

    # pylint: disable=no-member
    @app.route("/upload", methods=["GET", "POST"])
    def upload_file():
        if request.method == "POST":
            if "image" not in request.files:
                app.logger.error("image not in request.files")
                return jsonify(success=False), 500
            image = request.files["image"]
            if image.filename == "":
                app.logger.error("no selected file")
                return jsonify(success=False), 500
            if image and allowed_file(image.filename):
                filename = secure_filename(image.filename)
                path = os.path.join(os.environ["DEADEYE_UPLOAD_DIR"], filename)
                image.save(path)
                app.logger.info(f"uploaded image: {path}")
                return jsonify(success=True)

        return """
        <!doctype html>
        <head><title>Upload Image</title></head>
        <h1>Upload Image</h1>
        <form method=post enctype=multipart/form-data>
        <input type=file name=file>
        <input type=submit value=Upload>
        </form>
        """

    @app.after_request
    def after_request(response):
        response.headers.add("Access-Control-Allow-Origin", "*")
        response.headers.add("Access-Control-Allow-Headers", "*")
        response.headers.add("Access-Control-Allow-Methods", "*")
        return response

    return app
