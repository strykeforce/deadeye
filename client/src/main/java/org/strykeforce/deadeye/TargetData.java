package org.strykeforce.deadeye;

import com.squareup.moshi.JsonReader;
import okio.BufferedSource;

import java.io.IOException;
import java.util.Objects;

public class TargetData {
    private static final JsonReader.Options OPTIONS = JsonReader.Options.of("id", "sn", "v");

    private final String id;
    private final int serial;
    private final boolean valid;

    public TargetData(String id, int serial, boolean valid) {
        this.id = id;
        this.serial = serial;
        this.valid = valid;
    }

    public static TargetData of(BufferedSource source) throws IOException {
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
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        TargetData that = (TargetData) o;
        return serial == that.serial &&
                valid == that.valid &&
                id.equals(that.id);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, serial, valid);
    }

    @Override
    public String toString() {
        return "TargetData{" +
                "id='" + id + '\'' +
                ", serial=" + serial +
                ", valid=" + valid +
                '}';
    }
}
