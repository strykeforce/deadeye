package org.strykeforce.deadeye;

import com.squareup.moshi.JsonAdapter;
import com.squareup.moshi.Moshi;
import com.squareup.moshi.Types;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import org.jetbrains.annotations.NotNull;
import org.junit.jupiter.api.*;

import java.io.IOException;
import java.lang.reflect.Type;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Collections;
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
    void setUp() {
        Moshi moshi = new Moshi.Builder().build();
        Type type = Types.newParameterizedType(List.class, Link.Config.class);
        JsonAdapter<List<Link.Config>> jsonAdapter = moshi.adapter(type);
        List<Link.Config> configs = Collections.singletonList(DEFAULT);
        NetworkTable deadeyeTable = nti.getTable(DEADEYE_TABLE);
        entry = deadeyeTable.getEntry(LINK_ENTRY);
        entry.setString(jsonAdapter.toJson(configs));
        entry.setPersistent();
    }

    @AfterEach
    void tearDown() {
        entry.clearPersistent();
        nti.deleteAllEntries();
    }

    @Test
    void testGetConfigs() throws IOException {
        NetworkTable deadeyeTable = nti.getTable(DEADEYE_TABLE);
        List<Link.Config> configs = Link.getConfigs(deadeyeTable);
        assertEquals(1, configs.size());
    }

    @Test
    void testHasLinkAddresses() throws IOException {
        List<InetAddress> addresses = new ArrayList<>();
        addresses.add(InetAddress.getByName("10.27.67.6"));

        List<Link.Config> configs = new ArrayList<>();
        configs.add(DEFAULT);
        assertFalse(Link.hasLinkAddresses(addresses, configs));
        configs.add(new Link.Config("10.27.67.6", 1, true));
        assertTrue(Link.hasLinkAddresses(addresses, configs));
    }

    @Test
    void testDefaultLinkEntry() throws IOException {
        List<Link.Config> configs = getConfigs();

        assertEquals(1, configs.size());
        assertEquals(DEFAULT, configs.get(0));
    }

    @NotNull
    private List<Link.Config> getConfigs() throws IOException {
        NetworkTable deadeyeTable = nti.getTable(DEADEYE_TABLE);
        return Link.getConfigs(deadeyeTable);
    }

    @Test
    void testAddOwnLinkEntry() throws IOException {
        // side effect adds link config with this computer's IP address
        @SuppressWarnings("unused")
        Link link = new Link(nti);

        List<Link.Config> configs = getConfigs();

        // tests when adding own config
//        assertEquals(2, configs.size());
//        assertEquals(DEFAULT, configs.get(0));
//        assertNotEquals(DEFAULT, configs.get(1));

        // tests when replacing with own config
        assertEquals(1, configs.size());
        assertNotEquals(DEFAULT, configs.get(0));
    }


}