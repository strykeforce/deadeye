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
        nt_server = os.environ["DEADEYE_NT_SERVER"]
        self.logger.debug("connecting to NT server at %s", nt_server)

        NetworkTables.initialize(server=nt_server)
        NetworkTables.addConnectionListener(connection_listener, immediateNotify=True)
