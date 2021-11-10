package org.strykeforce.deadeye;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import okio.Buffer;
import okio.BufferedSource;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

@SuppressWarnings({"unused", "rawtypes"})
class Link extends Thread {

  static final String DEADEYE_TABLE = "/Deadeye";
  static final String LINK_ENTRY = "Link";

  static final Logger logger = LoggerFactory.getLogger(Link.class);

  private final Map<String, Deadeye> deadeyeCache = new HashMap<>();
  private final byte[] bytes = new byte[512];
  private final Buffer buffer = new Buffer();
  private final DatagramPacket packet = new DatagramPacket(bytes, bytes.length);
  private final int port;

  Link(@NotNull NetworkTableInstance networkTableInstance, @Nullable String address) {
    NetworkTable deadeyeTable = networkTableInstance.getTable(DEADEYE_TABLE);
    var config = LinkConfig.getConfig(deadeyeTable);
    if (address != null) {
      config = config.withAddress(address);
      logger.info("configuration overridden with address \"{}\" now:\n\t{}", address, config);
    }

    if (!config.isValid() && address == null) {
      logger.info("invalid config and no address supplied, attempting to detect interface address");
      List<InetAddress> siteLocalAddresses = LinkConfig.getSiteLocalAddresses();
      if (siteLocalAddresses.isEmpty()) {
        // really never expect this
        throw new IllegalStateException("no site local addresses!");
      }
      var detectedAddress = siteLocalAddresses.get(0).getHostAddress();
      if (siteLocalAddresses.size() > 1) {
        logger.warn("multiple site local addresses, using first: {}", detectedAddress);
      }
      config = config.withAddress(detectedAddress);
      logger.info("configuration overridden with address \"{}\" now:\n\t{}", detectedAddress,
          config);
    }

    config.save(deadeyeTable);

    port = config.port;
    setName("Link");
    setDaemon(true);
  }

  @Override
  public void run() {
    try {
      DatagramSocket socket = new DatagramSocket(port);
      //noinspection InfiniteLoopStatement
      while (true) {
        socket.receive(packet);
        String id = new String(packet.getData(), 0, 2);
        buffer.write(packet.getData(), 2, packet.getLength() - 2);
        Deadeye handler = deadeyeCache.get(id);
        if (handler != null) {
          handler.handleTargetData(buffer);
        } else {
          logger.warn("unrecognized target data id: {}", id);
        }
      }
    } catch (IOException e) {
      logger.error("Deadeye target data receive thread", e);
    }
  }

  synchronized void addTargetDataHandler(String id, Deadeye handler) {
    deadeyeCache.put(id, handler);
  }

  void sendTest(String id, BufferedSource source) throws IOException {
    deadeyeCache.get(id).handleTargetData(source);
  }

}
