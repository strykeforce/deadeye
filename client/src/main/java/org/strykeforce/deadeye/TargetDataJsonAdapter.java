package org.strykeforce.deadeye;

import com.squareup.moshi.JsonReader;
import com.squareup.moshi.JsonWriter;
import okio.Buffer;
import okio.BufferedSource;

import java.io.IOException;

public class TargetDataJsonAdapter implements JsonAdapter<TargetData> {
    private static final JsonReader.Options OPTIONS = JsonReader.Options.of("id", "sn", "v");

    @Override
    public TargetData fromJson(BufferedSource source) throws IOException {
        JsonReader reader = JsonReader.of(source);
        String id = null;
        int serial = -1;
        boolean valid = false;

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
                case -1:
                    reader.skipName();
                    reader.skipValue();
                    break;
                default:
                    throw new IllegalStateException("Unexpected value: " + reader.selectName(OPTIONS));
            }
        }
        reader.endObject();
        return new TargetData(id, serial, valid);
    }

    @Override
    public String toJson(TargetData targetData) throws IOException {
        Buffer buffer = new Buffer();
        JsonWriter writer = JsonWriter.of(buffer);
        writer.beginObject();
        writer.name("id").value(targetData.id);
        writer.name("sn").value(targetData.serial);
        writer.name("v").value(targetData.valid);
        writer.endObject();
        return buffer.readUtf8();
    }
}
