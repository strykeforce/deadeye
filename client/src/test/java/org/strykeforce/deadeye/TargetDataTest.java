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
        @SuppressWarnings("unchecked") DeadeyeJsonAdapter<TargetData> jsonAdapter =
                new TargetData().getJsonAdapter();
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
        @SuppressWarnings("unchecked") DeadeyeJsonAdapter<TargetData> jsonAdapter =
                new TargetData().getJsonAdapter();
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

    @Test
    void testMinAreaRectTargetDataDeserialization() throws IOException {
        Buffer buffer = new Buffer();
        buffer.writeUtf8("{\"id\":\"Z1\",\"d\":[1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0," +
                "12.0,13.0],\"sn\":3,\"v\":true}");

        double angle = 1;
        Point2D center = new Point2D(2, 3);
        double height = 4;
        double width = 5;
        Point2D[] points = new Point2D[]{
                new Point2D(6, 7),
                new Point2D(8, 9),
                new Point2D(10, 11),
                new Point2D(12, 13),
        };
        MinAreaRectTargetData exp = new MinAreaRectTargetData("Z1", 3, true, angle, center, width,
                height, points);
        @SuppressWarnings("unchecked") DeadeyeJsonAdapter<MinAreaRectTargetData> jsonAdapter =
                new MinAreaRectTargetData().getJsonAdapter();
        MinAreaRectTargetData td = jsonAdapter.fromJson(buffer);
        assertEquals(exp, td);
    }

    @Test
    void testMinAreaRectTargetDataSerialization() throws IOException {
        double angle = 1;
        Point2D center = new Point2D(2, 3);
        double height = 4;
        double width = 5;
        Point2D[] points = new Point2D[]{
                new Point2D(6, 7),
                new Point2D(8, 9),
                new Point2D(10, 11),
                new Point2D(12, 13),
        };
        MinAreaRectTargetData expected = new MinAreaRectTargetData("Z1", 3, true, angle, center,
                width,
                height, points);

        @SuppressWarnings("unchecked") DeadeyeJsonAdapter<MinAreaRectTargetData> jsonAdapter =
                new MinAreaRectTargetData().getJsonAdapter();
        String json = jsonAdapter.toJson(expected);
        Buffer buffer = new Buffer();
        buffer.writeUtf8(json);

        MinAreaRectTargetData td = jsonAdapter.fromJson(buffer);
        assertEquals(expected, td);
    }


}