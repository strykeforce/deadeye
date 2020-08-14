package org.strykeforce.deadeye;

import com.squareup.moshi.JsonReader;
import com.squareup.moshi.JsonWriter;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import okio.Buffer;
import org.jetbrains.annotations.NotNull;
import org.junit.jupiter.api.*;

import java.io.IOException;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;
import static org.strykeforce.deadeye.Link.DEADEYE_TABLE;
import static org.strykeforce.deadeye.Link.LINK_ENTRY;

class LinkTest {

    private static final NetworkTableInstance nti = NetworkTableInstance.create();
    private static final Link.Config DEFAULT = new Link.Config("10.27.67.2", 2767, true);

    private NetworkTableEntry entry;

    @BeforeAll
    static void beforeAll() {
        nti.startLocal();
    }

    @AfterAll
    static void afterAll() {
        nti.stopLocal();
        nti.close();
    }

    @BeforeEach
    void setUp() throws IOException {
        Buffer buffer = new Buffer();
        JsonWriter writer = JsonWriter.of(buffer);

        writer.beginArray();
        DEFAULT.writeJson(writer);
        writer.endArray();
        writer.close();

        NetworkTable deadeyeTable = nti.getTable(DEADEYE_TABLE);
        entry = deadeyeTable.getEntry(LINK_ENTRY);
        entry.setString(buffer.readUtf8());
        entry.setPersistent();
    }

    @AfterEach
    void tearDown() {
        entry.clearPersistent();
        nti.deleteAllEntries();
    }

    @Test
    void testHasLinkAddresses() throws IOException {
        List<InetAddress> addresses = new ArrayList<>();
        addresses.add(InetAddress.getByName("10.27.67.6"));

        List<Link.Config> configs = new ArrayList<>();
        configs.add(DEFAULT);
        assertFalse(Link.hasLinkAddresses(addresses, configs));
        configs.add(new Link.Config("10.27.67.6",1, true));
        assertTrue(Link.hasLinkAddresses(addresses, configs));
    }

    @NotNull
    private List<Link.Config> getConfigs() throws IOException {
        Buffer buffer = new Buffer();
        JsonReader reader = JsonReader.of(buffer);

        buffer.writeUtf8(entry.getString("[]"));
        return Link.Config.readConfigs(reader);
    }

    @Test
    void testDefaultLinkEntry() throws IOException {
        List<Link.Config> configs = getConfigs();

        assertEquals(1, configs.size());
        assertEquals(DEFAULT, configs.get(0));
    }

    @Test
    void testAddOwnLinkEntry() throws IOException {
        // adds link config with this computer's IP address
        Link link = new Link(nti);

        List<Link.Config> configs = getConfigs();

        assertEquals(2, configs.size());
        assertEquals(DEFAULT, configs.get(0));
        assertNotEquals(DEFAULT, configs.get(1));
    }


}