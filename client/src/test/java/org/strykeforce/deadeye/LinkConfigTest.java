package org.strykeforce.deadeye;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.strykeforce.deadeye.Link.DEADEYE_TABLE;
import static org.strykeforce.deadeye.Link.LINK_ENTRY;

import com.squareup.moshi.JsonAdapter;
import com.squareup.moshi.Moshi;
import com.squareup.moshi.Types;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import java.io.IOException;
import java.lang.reflect.Type;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Collections;
import java.util.List;
import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;

class LinkConfigTest {

  private static final NetworkTableInstance nti = NetworkTableInstance.create();
  private static final LinkConfig DEFAULT = new LinkConfig("10.27.67.2", 2767, true);

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
    Type type = Types.newParameterizedType(List.class, LinkConfig.class);
    JsonAdapter<List<LinkConfig>> jsonAdapter = moshi.adapter(type);
    List<LinkConfig> configs = Collections.singletonList(DEFAULT);
    NetworkTable deadeyeTable = nti.getTable(DEADEYE_TABLE);
    entry = deadeyeTable.getEntry(LINK_ENTRY);
    entry.setString(jsonAdapter.toJson(configs));
    entry.setPersistent();
  }

  @AfterEach
  void tearDown() {
    entry.clearPersistent();
    // FIXME: delete all test entries
  }

  @Test
  void testNoLinkEntry() throws IOException {
    tearDown();
    nti.flush();
    NetworkTable deadeyeTable = nti.getTable(DEADEYE_TABLE);
    assertFalse(deadeyeTable.containsKey(LINK_ENTRY));
    LinkConfig config = LinkConfig.getConfig(deadeyeTable);
    assertEquals(LinkConfig.DEFAULT, config);
  }

  @Test
  void testEmptyLinkEntry() throws IOException {
    entry.setString("");
    NetworkTable deadeyeTable = nti.getTable(DEADEYE_TABLE);
    LinkConfig config = LinkConfig.getConfig(deadeyeTable);
    assertEquals(LinkConfig.DEFAULT, config);
  }

  @Test
  void testEmptyLinkList() throws IOException {
    entry.setString("[]");
    NetworkTable deadeyeTable = nti.getTable(DEADEYE_TABLE);
    LinkConfig config = LinkConfig.getConfig(deadeyeTable);
    assertEquals(LinkConfig.DEFAULT, config);
  }

  @ParameterizedTest
  @ValueSource(strings = {
      "[{ \"port\": 5800, \"enabled\": false}]",
      "[{ \"address\": \"10.27.67.2\", \"enabled\": false}]",
      "[{ \"address\": \"10.27.672\", \"port\": 5801, \"enabled\": false}]",
      "[{ \"address\": \"\", \"port\": 5800, \"enabled\": false}]",
      "[{ \"address\": \"abc\", \"port\": 5800, \"enabled\": false}]",
      "[{ \"address\": null, \"port\": 5800, \"enabled\": false}]",
      "[{ \"address\": \"10.27.67.2\", \"port\": null, \"enabled\": false}]",
      "[{ \"address\": \"10.27.67.2\", \"port\": 5800, \"enabled\": \"false\"}]",
      "[{ \"address\": \"10.27.67.2\", \"port\": 5800, \"enabled\": null}]",
  })
  void testMalformedLinkEntry(String linkEntry) throws IOException {
    entry.setString(linkEntry);
    NetworkTable deadeyeTable = nti.getTable(DEADEYE_TABLE);
    LinkConfig config = LinkConfig.getConfig(deadeyeTable);
    assertEquals(LinkConfig.DEFAULT, config);
  }

  @Test
  void testMultipleLinks() {
    entry.setString("[{ \"address\": \"1.2.3.4\", \"port\": 5801, \"enabled\": false},"
        + "{ \"address\": \"10.27.67.2\", \"port\": 5800, \"enabled\": true}]");
    NetworkTable deadeyeTable = nti.getTable(DEADEYE_TABLE);
    LinkConfig config = LinkConfig.getConfig(deadeyeTable);
    assertEquals(new LinkConfig("1.2.3.4", 5801, false), config);
  }

  @Test
  void testGetConfig() throws IOException {
    NetworkTable deadeyeTable = nti.getTable(DEADEYE_TABLE);
    LinkConfig config = LinkConfig.getConfig(deadeyeTable);
    assertEquals(DEFAULT, config);
  }

  @Test
  void testIsValid() {
    var siteLocalAddresses = LinkConfig.getSiteLocalAddresses();
    assertTrue(siteLocalAddresses.size() > 0);
    var address = siteLocalAddresses.get(0);
    var config = new LinkConfig(address.getHostAddress(), 5800, true);
    config = config.withAddress(address.getHostAddress());
    assertTrue(config.isValid());
  }

  @Test
  void testIsNotValid() {
    NetworkTable deadeyeTable = nti.getTable(DEADEYE_TABLE);
    var config = LinkConfig.getConfig(deadeyeTable);
    assertFalse(config.isValid());
  }

  @Test
  void testAddressToInetAddress() throws UnknownHostException {
    var config = new LinkConfig("10.1.1.1", 5800, true);
    var address = LinkConfig.addressToInetAddress(config.address);
    var expected = InetAddress.getByAddress(new byte[]{10, 1, 1, 1});
    assertEquals(expected, address);
  }

  @Test
  void testGetInetAddressMalformed() throws UnknownHostException {
    var config = new LinkConfig("10.1.1.1.1", 5800, true);
    var address = LinkConfig.addressToInetAddress(config.address);
    var expected = InetAddress.getByName("10.27.67.2");
    assertEquals(expected, address);
  }

  @Test
  void testDefaultLinkEntry() throws IOException {
    NetworkTable deadeyeTable = nti.getTable(DEADEYE_TABLE);
    var config = LinkConfig.getConfig(deadeyeTable);
    assertEquals(DEFAULT, config);
  }

  @Test
  void testSave() {
    NetworkTable deadeyeTable = nti.getTable(DEADEYE_TABLE);
    var config = LinkConfig.getConfig(deadeyeTable);
    assertEquals(DEFAULT, config);
    LinkConfig expected = new LinkConfig("1.2.3.4", 1111, true);
    expected.save(deadeyeTable);
    config = LinkConfig.getConfig(deadeyeTable);
    assertEquals(expected, config);
  }

}