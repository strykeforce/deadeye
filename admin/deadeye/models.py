# pylint: disable=no-member
import json
from networktables import NetworkTables, NetworkTablesInstance
from flask import current_app


class Unit:

    units = {}
    api = None

    def __init__(self, unit_id, camera_inums):
        self.cameras = {}
        self.id = unit_id
        for inum in camera_inums:
            try:
                cam = Camera(unit_id, inum)
                self.cameras[inum] = cam
            except Exception as e:
                current_app.logger.error(
                    "error loading Camera %s%s: %s", unit_id, inum, e
                )

    @classmethod
    def init(cls, api):
        Unit.api = api
        deadeye_table = NetworkTables.getTable("/Deadeye")
        unit_ids = deadeye_table.getSubTables()

        for unit_id in unit_ids:
            unit_table = NetworkTables.getTable(f"/Deadeye/{unit_id}")
            camera_inums = unit_table.getSubTables()
            cls.units[unit_id] = Unit(unit_id, camera_inums)

    def __repr__(self):
        return f"Unit({self.id})"


class Camera:
    def __init__(self, unit_id, inum):
        self.unit = unit_id
        self.inum = int(inum)
        self.id = f"{unit_id}{inum}"
        self.light = Light(self)
        self.on = self.table().getBoolean("On", False)
        self.error = self.table().getBoolean("Error", False)
        self.capture = self.load_json("Capture")
        self.pipeline = self.load_json("Pipeline")
        self.stream = self.load_json("Stream")
        self.info = self.load_json("Info")
        self.table().addEntryListenerEx(
            self.entry_listener, NetworkTablesInstance.NotifyFlags.UPDATE
        )

    def load_json(self, key):
        try:
            return json.loads(self.table().getString(key, ""))
        except json.JSONDecodeError as err:
            raise ValueError(f"Camera {key}: {err}")

    def enable(self, enabled):
        if self.on == enabled:
            return
        control_table = self.table()
        control_table.putBoolean("On", enabled)
        control_table.putBoolean("Off", not enabled)
        self.on = enabled
        Unit.api.refresh_units = True

    def set_capture(self, capture):
        capture_entry = self.table().getEntry("Capture")
        capture_entry.setString(json.dumps(capture))
        self.capture = capture
        Unit.api.refresh_units = True

    def set_pipeline(self, pipeline):
        pipeline_entry = self.table().getEntry("Pipeline")
        pipeline_entry.setString(json.dumps(pipeline))
        self.pipeline = pipeline
        Unit.api.refresh_units = True

    def set_stream(self, stream):
        stream_entry = self.table().getEntry("Stream")
        stream_entry.setString(json.dumps(stream))
        self.stream = stream
        Unit.api.refresh_units = True

    def table(self):
        return NetworkTables.getTable(f"/Deadeye/{self.unit}/{self.inum}")

    def entry_listener(self, table, key, value, is_new):
        del is_new, table  # unused
        if not value:
            return

        if key == "On":
            self.on = value
            self.error = False
        elif key == "Off":
            self.on = not value
            self.error = False
        elif key == "Error":
            self.error = value
        elif key == "Stream":
            self.stream = json.loads(value)
        elif key == "Pipeline":
            self.pipeline = json.loads(value)
        elif key == "Capture":
            self.capture = json.loads(value)
        elif key == "Info":
            self.info = json.loads(value)
        else:
            current_app.logger.error("unrecognized key: %s", key)

        Unit.api.refresh_units = True

    def __repr__(self):
        return f"Camera({self.unit}, {self.inum}"

    def __str__(self):
        on = self.on
        error = self.error
        return f"Camera {self.unit}{self.inum}: on={on} error={error}"


class Light:
    # properties other than "camera" will be stored in "__dict__" and therefore serialized
    __slots__ = ["camera", "__dict__"]

    def __init__(self, camera):
        control_table = camera.table().getSubTable("Light")
        self.camera = camera
        self.on = control_table.getBoolean("On", False)
        self.blink = control_table.getBoolean("Blink", False)
        control_table.addEntryListenerEx(
            self.entry_listener, NetworkTablesInstance.NotifyFlags.UPDATE
        )

    def enable(self, enabled):
        if self.on == enabled:
            return
        control_table = self.camera.table().getSubTable("Light")
        control_table.putBoolean("On", enabled)
        control_table.putBoolean("Off", not enabled)
        self.on = enabled
        Unit.api.refresh_units = True

    def entry_listener(self, table, key, value, is_new):
        del is_new, table  # unused
        if not value:
            return

        if key == "On":
            self.on = value
            self.blink = False
        elif key == "Off":
            self.on = not value
            self.blink = False
        elif key == "Blink":
            self.blink = value
        else:
            current_app.logger.error("unrecognized key: %s", key)

        Unit.api.refresh_units = True

    def __repr__(self):
        return f"Light({self.on}, {self.blink}"

    def __str__(self):
        return f"Light: on={self.on} blink={self.blink}"


class Link:
    def __init__(self, api):
        self.api = api
        deadeye_table = NetworkTables.getTable("/Deadeye")
        entry = deadeye_table.getString("Link", "[]")
        print(f"Link entry = {entry}")
        self.entries = json.loads(entry)
        deadeye_table.addEntryListenerEx(
            self.entry_listener, NetworkTablesInstance.NotifyFlags.UPDATE
        )

    def entry_listener(self, table, key, value, is_new):
        del is_new, table  # unused
        if not value:
            return

        if key == "Link":
            self.entries = json.loads(value)
        else:
            current_app.logger.error("unrecognized key: %s", key)

        self.api.refresh_link = True

    def set_entries(self, message):
        deadeye_table = NetworkTables.getTable("/Deadeye")
        entry = deadeye_table.getEntry("Link")
        entry.setString(json.dumps(message))
        self.entries = message
        self.api.refresh_link = True
