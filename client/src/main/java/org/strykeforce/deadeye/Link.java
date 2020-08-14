package org.strykeforce.deadeye;

import com.squareup.moshi.JsonReader;
import com.squareup.moshi.JsonWriter;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import okio.Buffer;
import org.jetbrains.annotations.NotNull;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.ArrayList;
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

    @NotNull
    List<Config> getConfigs(@NotNull NetworkTable deadeyeTable) throws IOException {
        NetworkTableEntry linkEntry = deadeyeTable.getEntry(LINK_ENTRY);
        Buffer buffer = new Buffer();
        buffer.writeUtf8(linkEntry.getString("[]"));
        JsonReader reader = JsonReader.of(buffer);
        return Config.readConfigs(reader);
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
        Buffer buffer = new Buffer();
        JsonWriter writer = JsonWriter.of(buffer);

        writer.beginArray();
        for (Config config : configs) {
            config.writeJson(writer);
        }
        writer.endArray();

        deadeyeTable.getEntry(LINK_ENTRY).setString(buffer.readUtf8());
    }

    static class Config {
        private static final JsonReader.Options OPTIONS = JsonReader.Options.of("address", "port", "enabled");
        private final String address;
        private final int port;
        private final boolean enabled;

        Config(@NotNull String address, int port, boolean enabled) {
            this.address = address;
            this.port = port;
            this.enabled = enabled;
        }

        @NotNull
        static List<Config> readConfigs(@NotNull JsonReader reader) throws IOException {
            List<Config> configs = new ArrayList<>();
            reader.beginArray();
            while (reader.hasNext())
                configs.add(Config.of(reader));
            reader.endArray();
            return configs;
        }

        @NotNull
        static Config of(@NotNull JsonReader reader) throws IOException {
            String address = null;
            int port = -1;
            boolean enabled = false;
            reader.beginObject();
            while (reader.hasNext()) {
                switch (reader.selectName(OPTIONS)) {
                    case 0:
                        address = reader.nextString();
                        break;
                    case 1:
                        port = reader.nextInt();
                        break;
                    case 2:
                        enabled = reader.nextBoolean();
                        break;
                    case -1:
                        reader.skipName();
                        reader.skipValue();
                        break;
                    default:
                        throw new IllegalStateException("Unexpected value: " + reader.selectName(OPTIONS));
                }
            }
            reader.endObject();
            assert address != null;
            return new Config(address, port, enabled);
        }

        void writeJson(@NotNull JsonWriter writer) throws IOException {
            writer.beginObject();
            writer.name("address").value(address);
            writer.name("port").value(port);
            writer.name("enabled").value(enabled);
            writer.endObject();
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
