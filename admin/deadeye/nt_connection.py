import os

from networktables import NetworkTables

from .settings import DEADEYE_NT_SERVER


class NetworkTablesConnection:
    def __init__(self, api):
        self.logger = api.app.logger

        self.connecting = False
        self.connected = False

    def connect(self, callback):
        if self.connected or self.connecting:
            self.logger.warn(
                "connect: connecting=%r, connected=%r",
                self.connecting,
                self.connected,
            )
            return

        def connection_listener(is_connected, info):
            self.connecting = False
            self.connected = is_connected

            if not is_connected:
                self.logger.debug("disconnected from NT server")
                return

            root = NetworkTables.getGlobalTable()
            if not root.containsSubTable("Deadeye"):
                self.app.logger.fatal("Deadeye subtable missing from Network Tables")
                NetworkTables.shutdown()
                callback(False)
                return

            if is_connected:
                self.logger.info("connected to NT server at %s", info.remote_ip)
            else:
                self.logger.error("connection to NT server failed: %s", info)

            callback(True)

        self.connecting = True
        self.logger.debug("connecting to NT server at %s", DEADEYE_NT_SERVER)

        NetworkTables.initialize(server=DEADEYE_NT_SERVER)
        NetworkTables.addConnectionListener(connection_listener, immediateNotify=True)
