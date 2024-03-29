package org.strykeforce.deadeye;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import com.squareup.moshi.JsonAdapter;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import okio.Buffer;
import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.strykeforce.deadeye.Deadeye.Capture;
import org.strykeforce.deadeye.Deadeye.Info;

class DeadeyeTest {

  private static final NetworkTableInstance nti = NetworkTableInstance.create();

  @BeforeAll
  static void beforeAll() {
    nti.startLocal();
  }

  @AfterAll
  static void afterAll() {
    nti.stopLocal();
    nti.close();
  }

  @AfterEach
  void tearDown() {
    NetworkTable table = nti.getTable(Link.DEADEYE_TABLE + "/Z/0");
    table.getEntry("Info").unpublish();
  }


  @Test
  void testId() {
    assertDoesNotThrow(() -> new Deadeye<>("A0", TargetData.class, nti));
    assertDoesNotThrow(() -> new Deadeye<>("a0", TargetData.class, nti));
    assertThrows(IllegalArgumentException.class, () -> new Deadeye<>("", TargetData.class, nti));
    assertThrows(IllegalArgumentException.class, () -> new Deadeye<>("A", TargetData.class, nti));
    assertThrows(IllegalArgumentException.class, () -> new Deadeye<>("0", TargetData.class, nti));
    assertThrows(IllegalArgumentException.class, () -> new Deadeye<>("A00", TargetData.class, nti));
    assertThrows(IllegalArgumentException.class, () -> new Deadeye<>("A5", TargetData.class, nti));

    Deadeye<TargetData> deadeye = new Deadeye<>("a1", TargetData.class, nti);
    assertEquals("A1", deadeye.getId());
  }

  @Test
  void testEnabled() {
    NetworkTable table = nti.getTable(Link.DEADEYE_TABLE + "/Z/0");
    Deadeye<TargetData> deadeye = new Deadeye<>("Z0", TargetData.class, nti);
    assertFalse(table.getEntry("On").getBoolean(false), "On");
    assertFalse(table.getEntry("Off").getBoolean(false), "Off");
    deadeye.setEnabled(true);
    assertTrue(table.getEntry("On").getBoolean(false), "On");
    deadeye.setEnabled(false);
    assertTrue(table.getEntry("Off").getBoolean(true), "Off");
  }

  @Test
  void testLightEnabled() {
    NetworkTable table = nti.getTable(Link.DEADEYE_TABLE + "/Z/0/Light");
    Deadeye<TargetData> deadeye = new Deadeye<>("Z0", TargetData.class, nti);
    assertFalse(table.getEntry("On").getBoolean(false), "On");
    assertFalse(table.getEntry("Off").getBoolean(false), "Off");
    deadeye.setLightEnabled(true);
    assertTrue(table.getEntry("On").getBoolean(false), "On");
    deadeye.setLightEnabled(false);
    assertTrue(table.getEntry("Off").getBoolean(true), "Off");
  }

  @Test
  void testGetInfo() {
    Info expected = new Info(false, "TestPipeline", "1.0.0");
    NetworkTable table = nti.getTable(Link.DEADEYE_TABLE + "/Z/0");
    NetworkTableEntry entry = table.getEntry("Info");
    JsonAdapter<Info> jsonAdapter = Deadeye.getInfoJsonAdapter();
    entry.setString(jsonAdapter.toJson(expected));

    Deadeye<TargetData> deadeye = new Deadeye<>("Z0", TargetData.class, nti);
    Info actual = deadeye.getInfo();
    assertEquals(expected, actual);
  }

  @Test
  void testGetNoInfo() {
    Deadeye<TargetData> deadeye = new Deadeye<>("Z0", TargetData.class, nti);
    Deadeye.Info actual = deadeye.getInfo();
    assertEquals("ERROR", actual.pipeline);
  }

  @Test
  void testGetCapture() {
    Capture expected = new Capture("test", 30, 640, 480);
    NetworkTable table = nti.getTable(Link.DEADEYE_TABLE + "/Z/0");
    NetworkTableEntry entry = table.getEntry("Capture");
    entry.setString("{\n"
            + "  \"config\": {\n"
            + "    \"captureWidth\": 3264,\n"
            + "    \"captureHeight\": 2464,\n"
            + "    \"ispDigitalGainRange\": \"1 8\"\n"
            + "  },\n"
            + "  \"fps\": 30,\n"
            + "  \"h\": 480,\n"
            + "  \"type\": \"test\",\n"
            + "  \"w\": 640,\n"
            + "  \"pattern\": \"ball\"\n"
            + "}\n");
    Deadeye<TargetData> deadeye = new Deadeye<>("Z0", TargetData.class, nti);
    Capture actual = deadeye.getCapture();
    assertEquals(expected, actual);
  }

  @Test
  void testGetNoCapture() {
    Deadeye<TargetData> deadeye = new Deadeye<>("Z0", TargetData.class, nti);
    Deadeye.Capture actual = deadeye.getCapture();
    assertEquals("ERROR", actual.type);
  }

  @Test
  void testTargetDataListener() throws IOException, InterruptedException {
    TargetData expected = new TargetData("Z1", 2767, true);
    @SuppressWarnings("unchecked") DeadeyeJsonAdapter<TargetData> jsonAdapter = new TargetData().getJsonAdapter();

    Buffer buffer = new Buffer();
    buffer.writeUtf8(jsonAdapter.toJson(expected));
    CountDownLatch latch = new CountDownLatch(1);

    Deadeye<TargetData> deadeye = new Deadeye<>("Z1", TargetData.class, nti);
    deadeye.setTargetDataListener(td -> {
      assertEquals(expected, td);
      latch.countDown();
    });
    Link link = deadeye.getLink();
    link.sendTest("Z1", buffer);

    assertEquals(expected, deadeye.getTargetData());
    assertTrue(latch.await(1, TimeUnit.SECONDS), "timed out");
  }

  @Test
  void testUprightRectTargetDataListener() throws IOException, InterruptedException {
    Point tl = new Point(1, 2);
    Point br = new Point(3, 4);
    Point c = new Point(5, 6);
    UprightRectTargetData expected = new UprightRectTargetData("Z2", 3, true, tl, br, c);
    @SuppressWarnings("unchecked") DeadeyeJsonAdapter<UprightRectTargetData> jsonAdapter =
            new UprightRectTargetData().getJsonAdapter();

    Buffer buffer = new Buffer();
    buffer.writeUtf8(jsonAdapter.toJson(expected));
    CountDownLatch latch = new CountDownLatch(1);

    Deadeye<UprightRectTargetData> deadeye = new Deadeye<>("Z2", UprightRectTargetData.class, nti);
    deadeye.setTargetDataListener(td -> {
      assertEquals(expected, td);
      latch.countDown();
    });
    Link link = deadeye.getLink();
    link.sendTest("Z2", buffer);

    assertEquals(expected, deadeye.getTargetData());
    assertTrue(latch.await(1, TimeUnit.SECONDS), "timed out");
  }

  @Test
  void testLinkThread() throws IOException, InterruptedException {
    TargetData expected = new TargetData("Z3", 2767, true);
    @SuppressWarnings("unchecked") DeadeyeJsonAdapter<TargetData> jsonAdapter = new TargetData().getJsonAdapter();
    Deadeye<TargetData> deadeye = new Deadeye<>("Z3", TargetData.class, nti);
    String json = jsonAdapter.toJson(expected);
    String payload = "Z3" + json;

    int counts = 100;
    CountDownLatch latch = new CountDownLatch(counts);

    deadeye.setTargetDataListener(data -> {
      assertEquals(expected, data);
      latch.countDown();
    });

    DatagramPacket packet =
            new DatagramPacket(payload.getBytes(), payload.getBytes().length,
                    InetAddress.getLoopbackAddress(), LinkConfig.DEFAULT.port);
    DatagramSocket socket = new DatagramSocket();
    for (int i = 0; i < counts; i++) {
      socket.send(packet);
      Thread.sleep(5);
    }

    assertEquals(expected, deadeye.getTargetData());

    assertTrue(latch.await(1, TimeUnit.SECONDS), "CountDownLatch timeout");
  }

}
