package org.strykeforce.deadeye;

import com.squareup.moshi.JsonReader;
import com.squareup.moshi.JsonWriter;
import java.io.IOException;
import java.util.Objects;
import okio.Buffer;
import okio.BufferedSource;
import org.jetbrains.annotations.NotNull;

/**
 * {@code TargetData} is an base class for target data sent by Deadeye camera pipelines.
 *
 * @see UprightRectTargetData
 * @see MinAreaRectTargetData
 * @see TargetListTargetData
 */
@SuppressWarnings("unused")
public class TargetData {

  /** Deadeye unit ID. */
  @NotNull public final String id;
  /** Serial number of target data message. */
  public final int serial;
  /** True if target data is valid. */
  public final boolean valid;

  /**
   * Constructs and initializes an invalid {@code TargetData} with no id and serial 0.
   */
  public TargetData() {
    this("", 0, false);
  }

  /**
   * Construct and initialize a {@code TargetData} with the specified values.
   * @param id the camera id.
   * @param serial  the incrementing serial identifier of the target data.
   * @param valid   true if a valid target was detected.
   */
  public TargetData(@NotNull String id, int serial, boolean valid) {
    this.id = id;
    this.serial = serial;
    this.valid = valid;
  }

  @SuppressWarnings("rawtypes")
  public DeadeyeJsonAdapter getJsonAdapter() {
    return new JsonAdapterImpl();
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (o == null || getClass() != o.getClass()) return false;
    TargetData that = (TargetData) o;
    return serial == that.serial && valid == that.valid && id.equals(that.id);
  }

  @Override
  public int hashCode() {
    return Objects.hash(id, serial, valid);
  }

  @Override
  public String toString() {
    return "TargetData{" + "id='" + id + '\'' + ", serial=" + serial + ", valid=" + valid + '}';
  }

  private static class JsonAdapterImpl implements DeadeyeJsonAdapter<TargetData> {
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
}
