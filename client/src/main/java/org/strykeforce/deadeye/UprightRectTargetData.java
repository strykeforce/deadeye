package org.strykeforce.deadeye;

import com.squareup.moshi.JsonReader;
import com.squareup.moshi.JsonWriter;
import java.io.IOException;
import java.util.Objects;
import okio.Buffer;
import okio.BufferedSource;
import org.jetbrains.annotations.NotNull;

/**
 * A <code>UprightRectTargetData</code> represents data returned from a Deadeye
 * <code>UprightRectPipeline</code>.
 */
@SuppressWarnings("unused")
public class UprightRectTargetData extends TargetData {

  static final int DATA_LENGTH = 6;

  /**
   * Top left corner <code>Point</code> of the upright bounding box enclosing this target.
   */
  @NotNull
  public final Point topLeft;
  /**
   * Bottom right corner <code>Point</code> of the upright bounding box enclosing this target.
   */
  @NotNull
  public final Point bottomRight;
  /**
   * Center <code>Point</code> of the upright bounding box enclosing this target.
   */
  @NotNull
  public final Point center;

  /**
   * Constructs and initializes an invalid <code>UprightRectTargetData</code> with no id, serial 0,
   * and zero-area bounding box at the origin.
   */
  public UprightRectTargetData() {
    this("", 0, false, new Point(0, 0), new Point(0, 0), new Point(0, 0));
  }

  /**
   * Constructs and initializes a <code>UprightRectTargetData</code> with the specified values.
   *
   * @param id the <code>UprightRectPipeline</code> camera ID.
   * @param serial  the incrementing serial identifier of the target data.
   * @param valid   true if a valid target was detected.
   * @param topLeft the top left corner of the bounding box enclosing this target.
   * @param bottomRight the top left corner of the bounding box enclosing this target.
   * @param center the center of the bounding box enclosing this target.
   */
  public UprightRectTargetData(
      @NotNull String id,
      int serial,
      boolean valid,
      @NotNull Point topLeft,
      @NotNull Point bottomRight,
      @NotNull Point center) {
    super(id, serial, valid);
    this.topLeft = topLeft;
    this.bottomRight = bottomRight;
    this.center = center;
  }

  /**
   * Gets the area of the upright bounding box surrounding this target.
   *
   * @return the area of the bounding box.
   */
  public int area() {
    return width() * height();
  }

  /**
   * Gets the width of the upright bounding box surrounding this target.
   *
   * @return the width of the bounding box.
   */
  public int width() {
    return bottomRight.x - topLeft.x;
  }

  /**
   * Gets the height of the upright bounding box surrounding this target.
   *
   * @return the height of the bounding box.
   */
  public int height() {
    return bottomRight.y - topLeft.y;
  }

  @Override
  @SuppressWarnings("rawtypes")
  public DeadeyeJsonAdapter getJsonAdapter() {
    return new JsonAdapterImpl();
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }
    if (!super.equals(o)) {
      return false;
    }
    UprightRectTargetData that = (UprightRectTargetData) o;
    return topLeft.equals(that.topLeft)
        && bottomRight.equals(that.bottomRight)
        && center.equals(that.center);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), topLeft, bottomRight, center);
  }

  @Override
  public String toString() {
    return "UprightRectTargetData{"
        + "topLeft="
        + topLeft
        + ", bottomRight="
        + bottomRight
        + ", center="
        + center
        + "} "
        + super.toString();
  }

  private static class JsonAdapterImpl implements DeadeyeJsonAdapter<UprightRectTargetData> {

    // json d field: bb.tl().x, bb.tl().y, bb.br().x, bb.br().y, center.x, center.y
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
      return new UprightRectTargetData(
          Objects.requireNonNull(id), serial, valid, topLeft, bottomRight, center);
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
}
