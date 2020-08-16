package org.strykeforce.deadeye;

import okio.Buffer;
import org.junit.jupiter.api.Test;

import java.io.IOException;

import static org.junit.jupiter.api.Assertions.assertEquals;

class TargetDataTest {

    @Test
    void testTargetDataDeserialization() throws IOException {
        Buffer buffer = new Buffer();
        buffer.writeUtf8("{\"id\":\"Z1\",\"sn\":1,\"v\":true}");

        TargetData exp = new TargetData("Z1", 1, true);
        @SuppressWarnings("unchecked") DeadeyeJsonAdapter<TargetData> jsonAdapter = new TargetData().getJsonAdapter();
        TargetData td = jsonAdapter.fromJson(buffer);
        assertEquals(exp, td);

        buffer.writeUtf8("{\"foo\":\"bar\",\"id\":\"Z1\",\"d\":[1,2],\"sn\":2,\"v\":true}");
        exp = new TargetData("Z1", 2, true);
        td = jsonAdapter.fromJson(buffer);
        assertEquals(exp, td);
    }

    @Test
    void testTargetDataSerialization() throws IOException {
        TargetData expected = new TargetData("Z1", 1, true);
        @SuppressWarnings("unchecked") DeadeyeJsonAdapter<TargetData> jsonAdapter = new TargetData().getJsonAdapter();
        String json = jsonAdapter.toJson(expected);
        Buffer buffer = new Buffer();
        buffer.writeUtf8(json);
        TargetData td = jsonAdapter.fromJson(buffer);
        assertEquals(expected, td);
    }

    @Test
    void testUprightRectTargetDataDeserialization() throws IOException {
        Buffer buffer = new Buffer();
        buffer.writeUtf8("{\"id\":\"Z1\",\"d\":[1,2,3,4,5,6],\"sn\":3,\"v\":true}");

        Point tl = new Point(1, 2);
        Point br = new Point(3, 4);
        Point c = new Point(5, 6);
        UprightRectTargetData exp = new UprightRectTargetData("Z1", 3, true, tl, br, c);
        @SuppressWarnings("unchecked") DeadeyeJsonAdapter<UprightRectTargetData> jsonAdapter =
                new UprightRectTargetData().getJsonAdapter();
        UprightRectTargetData td = jsonAdapter.fromJson(buffer);
        assertEquals(exp, td);

        buffer.writeUtf8("{\"id\":\"Z1\",\"d\":[7,8,9,10,11,12],\"sn\":4,\"v\":true, \"foo\":99}");

        tl = new Point(7, 8);
        br = new Point(9, 10);
        c = new Point(11, 12);
        exp = new UprightRectTargetData("Z1", 4, true, tl, br, c);

        td = jsonAdapter.fromJson(buffer);
        assertEquals(exp, td);
    }

    @Test
    void testUprightRectTargetDataSerialization() throws IOException {
        Point tl = new Point(1, 2);
        Point br = new Point(3, 4);
        Point c = new Point(5, 6);
        UprightRectTargetData expected = new UprightRectTargetData("Z1", 3, true, tl, br, c);

        @SuppressWarnings("unchecked") DeadeyeJsonAdapter<UprightRectTargetData> jsonAdapter =
                new UprightRectTargetData().getJsonAdapter();
        String json = jsonAdapter.toJson(expected);
        Buffer buffer = new Buffer();
        buffer.writeUtf8(json);

        UprightRectTargetData td = jsonAdapter.fromJson(buffer);
        assertEquals(expected, td);
    }


}