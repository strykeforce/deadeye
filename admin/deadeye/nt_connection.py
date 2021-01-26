import os
from networktables import NetworkTables


class NetworkTablesConnection:
    def __init__(self, api):
        self.logger = api.app.logger

        self.connecting = False
        self.connected = False

    def connect(self, callback):
        if self.connected or self.connecting:
            self.logger.warn(
                "connect: connecting=%b, connected=%b",
                self.connecting,
                self.connected,
            )
            return

        def connection_listener(is_connected, info):
            self.nt_connected = is_connected
            self.nt_connecting = not is_connected
            if not is_connected:
                self.app.logger.debug("not connected, returning")
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
        nt_server = os.environ["DEADEYE_NT_SERVER"]
        self.logger.debug("connecting to NT server at %s", nt_server)

        NetworkTables.initialize(server=nt_server)
        NetworkTables.addConnectionListener(connection_listener, immediateNotify=True)
