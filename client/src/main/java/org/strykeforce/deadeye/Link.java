package org.strykeforce.deadeye;

import com.squareup.moshi.JsonAdapter;
import com.squareup.moshi.Moshi;
import com.squareup.moshi.Types;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import org.jetbrains.annotations.NotNull;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.lang.reflect.Type;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.List;
import java.util.Objects;
import java.util.function.Predicate;
import java.util.stream.Collectors;

class Link {
    static final String DEADEYE_TABLE = "/Deadeye";
    static final String LINK_ENTRY = "Link";

    static final Logger logger = LoggerFactory.getLogger(Link.class);

    Link(NetworkTableInstance networkTableInstance) {
        NetworkTable deadeyeTable = networkTableInstance.getTable(DEADEYE_TABLE);

        try {
            List<InetAddress> ownAddresses = getOwnInetAddresses();
            List<Config> configs = getConfigs(deadeyeTable);
            if (!hasLinkAddresses(ownAddresses, configs)) {
                logger.debug("Link config does not contains own address: {}", ownAddresses);
                InetAddress address = getOwnAddress(ownAddresses);
                addAddress(address, configs);
                saveConfigs(deadeyeTable, configs);
            } else {
                logger.debug("Link config contains own address: {}", ownAddresses);
            }
        } catch (IOException e) {
            logger.error("unable to initialize Link", e);
        }
    }

    @NotNull
    List<InetAddress> getOwnInetAddresses() throws SocketException {
        return NetworkInterface.networkInterfaces()
                .flatMap(NetworkInterface::inetAddresses)
                .filter(Inet4Address.class::isInstance)
                .filter(Predicate.not(InetAddress::isLoopbackAddress))
                .filter(Predicate.not(InetAddress::isLinkLocalAddress))
                .collect(Collectors.toList());
    }

    static List<Config> getConfigs(@NotNull NetworkTable deadeyeTable) throws IOException {
        NetworkTableEntry linkEntry = deadeyeTable.getEntry(LINK_ENTRY);
        String json = linkEntry.getString("[]");
        JsonAdapter<List<Config>> jsonAdapter = getConfigJsonAdapter();
        return jsonAdapter.fromJson(json);
    }

    static boolean hasLinkAddresses(@NotNull List<InetAddress> addresses, @NotNull List<Config> configs) throws IOException {
        for (Config config : configs) {
            InetAddress address = InetAddress.getByName(config.address);
            if (addresses.contains(address)) return true;
        }
        return false;
    }

    static InetAddress getOwnAddress(List<InetAddress> addresses) {
        if (addresses.size() != 1)
            throw new RuntimeException("Multiple Inet Addresses: " + addresses);
        return addresses.get(0);
    }

    static void addAddress(@NotNull InetAddress address, @NotNull List<Config> configs) {
        Config newConfig = new Config(address.getHostAddress(), 5800, true);
        configs.add(newConfig);
        logger.info("added Link config: {}", newConfig);
    }

    static void saveConfigs(@NotNull NetworkTable deadeyeTable, @NotNull List<Config> configs) throws IOException {
        JsonAdapter<List<Config>> jsonAdapter = getConfigJsonAdapter();
        deadeyeTable.getEntry(LINK_ENTRY).setString(jsonAdapter.toJson(configs));
    }

    private static JsonAdapter<List<Config>> getConfigJsonAdapter() {
        Moshi moshi = new Moshi.Builder().build();
        Type type = Types.newParameterizedType(List.class, Config.class);
        return moshi.adapter(type);
    }

    static class Config {
        private final String address;
        private final int port;
        private final boolean enabled;

        Config(@NotNull String address, int port, boolean enabled) {
            this.address = address;
            this.port = port;
            this.enabled = enabled;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            Config config = (Config) o;
            return port == config.port &&
                    enabled == config.enabled &&
                    address.equals(config.address);
        }

        @Override
        public int hashCode() {
            return Objects.hash(address, port, enabled);
        }

        @Override
        public String toString() {
            return "Config{" +
                    "address='" + address + '\'' +
                    ", port=" + port +
                    ", enabled=" + enabled +
                    '}';
        }
    }
}
