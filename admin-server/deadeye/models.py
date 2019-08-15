import json
from networktables import NetworkTables, NetworkTablesInstance
from flask import current_app


class Unit:

    units = {}
    api = None

    def __init__(self, unit_id, cameras):
        self.cameras = {}
        self.id = unit_id
        for inum in cameras:
            cam = Camera(unit_id, inum)
            self.cameras[inum] = cam

    @classmethod
    def init(cls, api):
        Unit.api = api
        deadeye_table = NetworkTables.getTable("/Deadeye")
        units = {sub[-1:] for sub in deadeye_table.getSubTables()}

        for unit_id in units:
            control_table = NetworkTables.getTable(f"/Deadeye/Control{unit_id}")
            cameras = {sub[-1:] for sub in control_table.getSubTables()}
            cls.units[unit_id] = Unit(unit_id, cameras)

    def __repr__(self):
        return f"Unit({self.id})"


class Camera:
    def __init__(self, unit_id, inum):
        self.unit = unit_id
        self.inum = int(inum)
        self.id = f"{unit_id}{inum}"
        control_table = NetworkTables.getTable(
            f"/Deadeye/Control{unit_id}/Camera{inum}"
        )
        self.on = control_table.getBoolean("On", False)
        self.error = control_table.getBoolean("Error", False)
        self.streamUrl = control_table.getString("StreamUrl", "")
        control_table.addEntryListenerEx(
            self.entry_listener, NetworkTablesInstance.NotifyFlags.UPDATE
        )

        config_table = NetworkTables.getTable(f"/Deadeye/Config{unit_id}")
        self.config = json.loads(config_table.getString(f"Camera{inum}", "{}"))

    def enable(self, enabled):
        control_table = NetworkTables.getTable(
            f"/Deadeye/Control{self.unit}/Camera{self.inum}"
        )
        control_table.putBoolean("On", enabled)
        control_table.putBoolean("Off", not enabled)
        self.on = enabled
        Unit.api.refresh = True

    def set_config(self, config):
        config_entry = NetworkTables.getEntry(
            f"/Deadeye/Config{self.unit}/Camera{self.inum}"
        )
        config_entry.setString(json.dumps(config))
        self.config = config
        Unit.api.refresh = True

    def entry_listener(self, table, key, value, is_new):
        del is_new  # unused

        if not value:
            return

        last = key[-1:]

        if last == "n":
            self.on = True
            self.error = False
        elif last == "f":
            self.on = False
            self.error = False
        elif last == "r":
            self.error = True
        else:
            current_app.logger.error("unrecognized key: %s", key)

        Unit.api.refresh = True

    def __repr__(self):
        return f"Camera({self.unit}, {self.inum}"

    def __str__(self):
        on = self.on
        error = self.error
        return f"Camera {self.unit}{self.inum}: on={on} error={error}"
