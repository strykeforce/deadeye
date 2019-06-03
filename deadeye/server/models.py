# pylint: disable=global-statement
import simplejson as json
from networktables import NetworkTables, NetworkTablesInstance
from app import app

NotifyFlags = NetworkTablesInstance.NotifyFlags

update_available = False


class Unit:

    units = {}

    def __init__(self, unit, cameras):
        self.cameras = []
        self.id = unit
        config_table = NetworkTables.getTable(f"/Deadeye/Config{unit}")
        self.ip = config_table.getString("IP", "0.0.0.0")
        for inum in cameras:
            cam = Camera(unit, inum)
            self.cameras.append(cam)

    @classmethod
    def init(cls):
        deadeye_table = NetworkTables.getTable("/Deadeye")
        units = {sub[-1:] for sub in deadeye_table.getSubTables()}
        app.logger.debug("units = %s", units)

        for unit in units:
            control_table = NetworkTables.getTable(f"/Deadeye/Control{unit}")
            app.logger.debug("control table = %s", control_table)
            cameras = {sub[-1:] for sub in control_table.getSubTables()}
            cls.units[unit] = Unit(unit, cameras)

    def __repr__(self):
        return f"Unit({self.id})"


class Camera:
    def __init__(self, unit_id, inum):
        self.unit = unit_id
        self.inum = inum
        self.id = f"{unit_id}{inum}"
        control_table = NetworkTables.getTable(
            f"/Deadeye/Control{unit_id}/Camera{inum}"
        )
        self.on = control_table.getBoolean("On", False)
        self.error = control_table.getBoolean("Error", False)
        control_table.addEntryListenerEx(self.entry_listener, NotifyFlags.UPDATE)

        config_table = NetworkTables.getTable(f"/Deadeye/Config{unit_id}")
        self.config = json.loads(config_table.getString(f"Camera{inum}", "{}"))

    def entry_listener(self, table, key, value, is_new):
        del is_new  # unused

        app.logger.debug("%s: %s: %s", table, key, value)

        if not value:
            app.logger.debug("%s set to False, skipping update", key)
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
            app.logger.error("unrecognized key: %s", key)

        global update_available
        update_available = True

    def __repr__(self):
        return f"Camera({self.unit}, {self.inum}"

    def __str__(self):
        on = self.on
        error = self.error
        return f"Camera {self.unit}{self.inum}: on={on} error={error}"


class UnitEncoder(json.JSONEncoder):
    def default(self, o):  # pylint: disable=method-hidden
        return o.__dict__
