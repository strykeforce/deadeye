package org.strykeforce.deadeye;

import com.squareup.moshi.JsonReader;
import okio.BufferedSource;

import java.io.IOException;
import java.util.Objects;

public class UprightRectTargetData extends TargetData {

    private static final JsonReader.Options OPTIONS = JsonReader.Options.of("id", "sn", "v", "d");
    //json d field: bb.tl().x, bb.tl().y, bb.br().x, bb.br().y, center.x, center.y
    private static final int DATA_LENGTH = 6;

    final Point topLeft;
    final Point bottomRight;
    final Point offset;

    public UprightRectTargetData(String id, int serial, boolean valid, Point topLeft, Point bottomRight, Point offset) {
        super(id, serial, valid);
        this.topLeft = topLeft;
        this.bottomRight = bottomRight;
        this.offset = offset;
    }

    public static UprightRectTargetData of(BufferedSource source) throws IOException {
        JsonReader reader = JsonReader.of(source);
        String id = null;
        int serial = -1;
        boolean valid = false;
        int[] data = new int[DATA_LENGTH];
        reader.beginObject();
        while (reader.hasNext()) {
            switch (reader.selectName(OPTIONS)) {
                case 0:
                    id = reader.nextString();
                    break;
                case 1:
                    serial = reader.nextInt();
                    break;
                case 2:
                    valid = reader.nextBoolean();
                    break;
                case 3:
                    reader.beginArray();
                    for (int i = 0; i < DATA_LENGTH; i++) {
                        data[i] = reader.nextInt();
                    }
                    reader.endArray();
                    break;
                case -1:
                    reader.skipName();
                    reader.skipValue();
                    break;
                default:
                    throw new IllegalStateException("Unexpected value: " + reader.selectName(OPTIONS));
            }
        }
        Point topLeft = new Point(data[0], data[1]);
        Point bottomRight = new Point(data[2], data[3]);
        Point center = new Point(data[4], data[5]);
        return new UprightRectTargetData(id, serial, valid, topLeft, bottomRight, center);
    }

    public int area() {
        return width() * height();
    }

    public int width() {
        return bottomRight.x - topLeft.x;
    }

    public int height() {
        return bottomRight.y - topLeft.y;
    }

    @Override
    public boolean equals(Object o) {
        if (this==o) return true;
        if (o==null || getClass()!=o.getClass()) return false;
        if (!super.equals(o)) return false;
        UprightRectTargetData that = (UprightRectTargetData) o;
        return topLeft.equals(that.topLeft) &&
                bottomRight.equals(that.bottomRight) &&
                offset.equals(that.offset);
    }

    @Override
    public int hashCode() {
        return Objects.hash(super.hashCode(), topLeft, bottomRight, offset);
    }

    @Override
    public String toString() {
        return "UprightRectTargetData{" +
                "topLeft=" + topLeft +
                ", bottomRight=" + bottomRight +
                ", offset=" + offset +
                "} " + super.toString();
    }
}
