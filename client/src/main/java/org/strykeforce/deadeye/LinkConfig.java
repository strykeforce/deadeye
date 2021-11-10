package org.strykeforce.deadeye;

import com.squareup.moshi.JsonAdapter;
import com.squareup.moshi.Moshi;
import com.squareup.moshi.Types;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import java.lang.reflect.Type;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.Collections;
import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;
import org.jetbrains.annotations.NotNull;

class LinkConfig {

  static final LinkConfig DEFAULT = new LinkConfig("10.27.67.2", 5800, true);
  final int port;
  final boolean enabled;
  final String address;

  LinkConfig(@NotNull String address, int port, boolean enabled) {
    this.address = address;
    this.port = port;
    this.enabled = enabled;
  }

  static LinkConfig getConfig(@NotNull NetworkTable deadeyeTable) {
    NetworkTableEntry linkEntry = deadeyeTable.getEntry(Link.LINK_ENTRY);
    String json = linkEntry.getString("[]");
    JsonAdapter<List<LinkConfig>> jsonAdapter = getConfigJsonAdapter();
    List<LinkConfig> configs;
    try {
      configs = jsonAdapter.fromJson(json);
    } catch (Exception e) {
      logConfigError(json);
      return DEFAULT;
    }
    if (configs == null || configs.size() < 1) {
      logConfigError(json);
      return DEFAULT;
    } else if (configs.size() > 1) {
      Link.logger.warn("more than one entry unsupported, using first:\n\t{}", json);
    }

    var config = configs.get(0);
    if (config.address == null || config.port == 0 || !config.address.equals(
        addressToInetAddress(config.address).getHostAddress())) {
      logConfigError(json);
      return DEFAULT;
    }

    Link.logger.info("parsed link from Deadeye NetworkTables:\n\t{}", config);
    return config;
  }

  private static void logConfigError(String json) {
    Link.logger.error(
        "entry not valid in Deadeye NetworkTables:\n\t\"{}\"\n\tfalling back to default: {}.",
        json, DEFAULT);
  }

  private static JsonAdapter<List<LinkConfig>> getConfigJsonAdapter() {
    Moshi moshi = new Moshi.Builder().build();
    Type type = Types.newParameterizedType(List.class, LinkConfig.class);
    return moshi.adapter(type);
  }

  static List<InetAddress> getSiteLocalAddresses() {
    try {
      return NetworkInterface.networkInterfaces()
          .flatMap(NetworkInterface::inetAddresses)
          .filter(Inet4Address.class::isInstance)
          .filter(InetAddress::isSiteLocalAddress)
          .collect(Collectors.toList());
    } catch (SocketException e) {
      Link.logger.error("Error getting own site local addresses", e);
      return Collections.emptyList();
    }
  }

  static InetAddress addressToInetAddress(String address) {
    InetAddress inetAddress = null;
    var invalid = true;
    try {
      inetAddress = InetAddress.getByName(address);
      // check address/port null, empty or malformed
      invalid = inetAddress.isLoopbackAddress() || !inetAddress.getHostAddress().equals(address);
    } catch (UnknownHostException e) {
      Link.logger.error("address parse error", e);
    }
    if (invalid) {
      Link.logger.error("invalid address: \"{}\"\n\tfalling back to default: {}", address,
          DEFAULT.address);
      inetAddress = getDefaultInetAddress();
    }
    return inetAddress;
  }

  static private InetAddress getDefaultInetAddress() {
    try {
      return InetAddress.getByName(DEFAULT.address);
    } catch (UnknownHostException ex) {
      throw new RuntimeException(ex); // should not get here
    }
  }

  LinkConfig withAddress(String address) {
    return new LinkConfig(address, this.port, this.enabled);
  }

  boolean isValid() {
    var inetAddress = addressToInetAddress(address);
    var addresses = getSiteLocalAddresses();
    boolean addressValid = addresses.contains(inetAddress);
    if (!addressValid) {
      Link.logger.error("configuration address \"{}\" not in {}\n\t" +
              "consider passing client IP address to Deadeye constructor.",
          inetAddress.getHostAddress(),
          addresses);
    }

    boolean portValid = port > 1024;
    if (!portValid) {
      Link.logger.error("port number invalid : {}", port);
    }

    if (!enabled) {
      Link.logger.warn("target data link disabled.");
    }

    return addressValid && portValid;
  }

  void save(NetworkTable deadeyeTable) {
    JsonAdapter<List<LinkConfig>> jsonAdapter = getConfigJsonAdapter();
    deadeyeTable.getEntry(Link.LINK_ENTRY)
        .setString(jsonAdapter.toJson(Collections.singletonList(this)));
    Link.logger.info("saved configuration: {}", this);
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }
    LinkConfig config = (LinkConfig) o;
    return port == config.port && enabled == config.enabled && address.equals(config.address);
  }

  @Override
  public int hashCode() {
    return Objects.hash(address, port, enabled);
  }

  @Override
  public String toString() {
    return "LinkConfig{" +
        "port=" + port +
        ", enabled=" + enabled +
        ", address='" + address + '\'' +
        '}';
  }
}
