package org.strykeforce.deadeye;

import com.squareup.moshi.JsonReader;
import com.squareup.moshi.JsonWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Objects;
import okio.Buffer;
import okio.BufferedSource;
import org.jetbrains.annotations.NotNull;

/**
 * A <code>TargetListTargetData</code> represents data returned from a Deadeye
 * <code>TargetListPipeline</code>.
 */
public class TargetListTargetData extends TargetData {

  static final int DATA_LENGTH = 5;

  /**
   * A <code>List</code> containing all <code>Target</code> instances identified by the
   * <code>TargetListPipeline</code>.
   */
  @NotNull
  public final List<Target> targets;

  /**
   * Constructs and initializes an invalid <code>TargetListTargetData</code> with no id, serial 0,
   * and no targets.
   */
  public TargetListTargetData() {
    this("", 0, false, Collections.emptyList());
  }

  /**
   * Constructs and initializes a <code>TargetListTargetData</code> with the specified values.
   *
   * @param id      the <code>TargetListPipeline</code> camera ID.
   * @param serial  the incrementing serial identifier of the target data.
   * @param valid   true if a valid target was detected.
   * @param targets the list of <code>Target</code> instances identified.
   */
  public TargetListTargetData(
      @NotNull String id, int serial, boolean valid, @NotNull List<Target> targets) {
    super(id, serial, valid);
    this.targets = targets;
  }

  @Override
  @NotNull
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
    TargetListTargetData that = (TargetListTargetData) o;
    return targets.equals(that.targets);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), targets);
  }

  @Override
  public String toString() {
    return "TargetListTargetData{" + "targets=" + targets + "} " + super.toString();
  }

  /**
   * A <code>Target</code> represents a single target returned in a <code>TargetListTargetData</code>.
   */
  public static class Target {

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
     * Gets the area of the contour enclosing the target.
     */
    public final int contourArea;

    /**
     * Constructs and initializes a <code>Target</code> with the specified values.
     *
     * @param topLeft the top left corner of the bounding box enclosing this target.
     * @param bottomRight the top left corner of the bounding box enclosing this target.
     * @param center the center of the bounding box enclosing this target.
     * @param contourArea the area of the contour enclosing the target.
     */
    public Target(
        @NotNull Point topLeft,
        @NotNull Point bottomRight,
        @NotNull Point center,
        int contourArea) {
      this.topLeft = topLeft;
      this.bottomRight = bottomRight;
      this.center = center;
      this.contourArea = contourArea;
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
    public boolean equals(Object o) {
      if (this == o) {
        return true;
      }
      if (o == null || getClass() != o.getClass()) {
        return false;
      }
      Target target = (Target) o;
      return contourArea == target.contourArea
          && topLeft.equals(target.topLeft)
          && bottomRight.equals(target.bottomRight)
          && center.equals(target.center);
    }

    @Override
    public int hashCode() {
      return Objects.hash(topLeft, bottomRight, center, contourArea);
    }

    @Override
    public String toString() {
      return "Target{"
          + "topLeft="
          + topLeft
          + ", bottomRight="
          + bottomRight
          + ", center="
          + center
          + ", contourArea="
          + contourArea
          + '}';
    }
  }

  private static class JsonAdapterImpl implements DeadeyeJsonAdapter<TargetListTargetData> {

    // json d field: bb.tl().x, bb.tl().y, bb.br().x, bb.br().y, center.x, center.y
    private static final JsonReader.Options OPTIONS = JsonReader.Options.of("id", "sn", "v", "d");

    @Override
    public TargetListTargetData fromJson(BufferedSource source) throws IOException {
      JsonReader reader = JsonReader.of(source);
      String id = null;
      int serial = -1;
      boolean valid = false;
      List<Target> targets = new ArrayList<>();

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
            while (reader.hasNext()) {
              int[] data = new int[DATA_LENGTH];
              reader.beginArray();
              for (int i = 0; i < DATA_LENGTH; i++) {
                data[i] = reader.nextInt();
              }
              reader.endArray();
              // bb.x, bb.y, bb.width, bb.height, area
              Point topLeft = new Point(data[0], data[1]);
              Point bottomRight = new Point(data[0] + data[2], data[1] + data[3]);
              Point center = Point.boundingBoxCenterFrom(topLeft, bottomRight);
              targets.add(new Target(topLeft, bottomRight, center, data[4]));
            }
            reader.endArray();
            break;
          default:
            throw new IllegalStateException("Unexpected value: " + reader.selectName(OPTIONS));
        }
      }
      reader.endObject();
      return new TargetListTargetData(Objects.requireNonNull(id), serial, valid, targets);
    }

    @Override
    public String toJson(TargetListTargetData targetData) throws IOException {
      Buffer buffer = new Buffer();
      JsonWriter writer = JsonWriter.of(buffer);
      writer.beginObject();
      writer.name("id").value(targetData.id);
      writer.name("sn").value(targetData.serial);
      writer.name("v").value(targetData.valid);

      writer.name("d").beginArray();
      for (Target t : targetData.targets) {
        writer.beginArray();
        writer.value(t.topLeft.x).value(t.topLeft.y);
        writer.value(t.width()).value(t.height());
        writer.value(t.contourArea);
        writer.endArray();
      }
      writer.endArray();
      writer.endObject();
      return buffer.readUtf8();
    }
  }
}
