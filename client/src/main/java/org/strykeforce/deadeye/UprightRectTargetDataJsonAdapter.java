package org.strykeforce.deadeye;

import com.squareup.moshi.JsonReader;
import com.squareup.moshi.JsonWriter;
import okio.Buffer;
import okio.BufferedSource;

import java.io.IOException;

import static org.strykeforce.deadeye.UprightRectTargetData.DATA_LENGTH;

public class UprightRectTargetDataJsonAdapter implements JsonAdapter<UprightRectTargetData> {

    //json d field: bb.tl().x, bb.tl().y, bb.br().x, bb.br().y, center.x, center.y
    private static final JsonReader.Options OPTIONS = JsonReader.Options.of("id", "sn", "v", "d");


    @Override
    public UprightRectTargetData fromJson(BufferedSource source) throws IOException {
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
        reader.endObject();
        Point topLeft = new Point(data[0], data[1]);
        Point bottomRight = new Point(data[2], data[3]);
        Point center = new Point(data[4], data[5]);
        return new UprightRectTargetData(id, serial, valid, topLeft, bottomRight, center);
    }

    @Override
    public String toJson(UprightRectTargetData targetData) throws IOException {
        Buffer buffer = new Buffer();
        JsonWriter writer = JsonWriter.of(buffer);
        writer.beginObject();
        writer.name("id").value(targetData.id);
        writer.name("sn").value(targetData.serial);
        writer.name("v").value(targetData.valid);
        writer.name("d").beginArray();
        writer.value(targetData.topLeft.x).value(targetData.topLeft.y);
        writer.value(targetData.bottomRight.x).value(targetData.bottomRight.y);
        writer.value(targetData.center.x).value(targetData.center.y);
        writer.endArray();
        writer.endObject();
        return buffer.readUtf8();

    }
}
