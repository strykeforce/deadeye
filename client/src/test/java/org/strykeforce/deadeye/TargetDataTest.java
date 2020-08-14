package org.strykeforce.deadeye;

import com.squareup.moshi.JsonReader;
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
        TargetData td = TargetData.of(buffer);
        assertEquals(exp, td);

        buffer.writeUtf8("{\"foo\":\"bar\",\"id\":\"Z1\",\"d\":[1,2],\"sn\":2,\"v\":true}");
        exp = new TargetData("Z1", 2, true);
        td = TargetData.of(buffer);
        assertEquals(exp, td);
    }

    @Test
    void testUprightRectTargetDataDeserialization() throws IOException {
        Buffer buffer = new Buffer();
        buffer.writeUtf8("{\"id\":\"Z1\",\"d\":[1,2,3,4,5,6],\"sn\":3,\"v\":true}");

        Point tl = new Point(1, 2);
        Point br = new Point(3, 4);
        Point c = new Point(5, 6);
        UprightRectTargetData exp = new UprightRectTargetData("Z1", 3, true, tl, br, c);
        UprightRectTargetData td = UprightRectTargetData.of(buffer);
        assertEquals(exp, td);

        buffer.writeUtf8("{\"id\":\"Z1\",\"d\":[7,8,9,10,11,12],\"sn\":4,\"v\":true, \"foo\":99}");

        tl = new Point(7, 8);
        br = new Point(9, 10);
        c = new Point(11, 12);
        exp = new UprightRectTargetData("Z1", 4, true, tl, br, c);
        td = UprightRectTargetData.of(buffer);
        assertEquals(exp, td);

    }
}