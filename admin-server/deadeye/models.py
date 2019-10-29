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
            cam = Camera(unit_id, inum)
            self.cameras[inum] = cam

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
        control_table = self.table()
        self.on = control_table.getBoolean("On", False)
        self.error = control_table.getBoolean("Error", False)
        self.stream = json.loads(control_table.getString("Stream", ""))
        self.pipeline = json.loads(control_table.getString("Pipeline", ""))
        control_table.addEntryListenerEx(
            self.entry_listener, NetworkTablesInstance.NotifyFlags.UPDATE
        )

    def enable(self, enabled):
        if self.on == enabled:
            return
        control_table = self.table()
        control_table.putBoolean("On", enabled)
        control_table.putBoolean("Off", not enabled)
        self.on = enabled
        Unit.api.refresh = True

    def set_pipeline(self, pipeline):
        pipeline_entry = self.table().getEntry("Pipeline")
        pipeline_entry.setString(json.dumps(pipeline))
        self.pipeline = pipeline
        Unit.api.refresh = True

    def set_stream(self, stream):
        stream_entry = self.table().getEntry("Stream")
        stream_entry.setString(json.dumps(stream))
        self.stream = stream
        Unit.api.refresh = True

    def table(self):
        return NetworkTables.getTable(f"/Deadeye/{self.unit}/{self.inum}")

    def entry_listener(self, table, key, value, is_new):
        del is_new  # unused
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
        else:
            current_app.logger.error("unrecognized key: %s", key)

        Unit.api.refresh = True

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
        Unit.api.refresh = True

    def entry_listener(self, table, key, value, is_new):
        del is_new  # unused
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

        Unit.api.refresh = True

    def __repr__(self):
        return f"Light({self.on}, {self.blink}"

    def __str__(self):
        return f"Light: on={self.on} blink={self.blink}"
