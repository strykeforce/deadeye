package org.strykeforce.deadeye;

import com.squareup.moshi.JsonReader;
import com.squareup.moshi.JsonWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
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
   * A <code>List</code> containing all {@code Rect} instances identified by the
   * <code>TargetListPipeline</code>.
   */
  @NotNull
  public final List<Rect> targets;

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
      @NotNull String id, int serial, boolean valid, @NotNull List<Rect> targets) {
    super(id, serial, valid);
    this.targets = targets;
  }

  /**
   * Returns a {@code List} of target {@code Rect}s in {@code Rect.topLeft.x} ascending order.
   *
   * @return a sorted {@code List}
   */
  public List<Rect> targetsOrderedByTopLeftX() {
    targets.sort(Comparator.comparingInt(r -> r.topLeft.x));
    return targets;
  }

  /**
   * Returns a {@code List} of target {@code Rect}s in {@code Rect.topLeft.y} ascending order.
   *
   * @return a sorted {@code List}
   */

  public List<Rect> targetsOrderedByTopLeftY() {
    targets.sort(Comparator.comparingInt(r -> r.topLeft.y));
    return targets;
  }

  /**
   * Returns a {@code List} of target {@code Rect}s in {@code Rect.center().x} ascending order.
   *
   * @return a sorted {@code List}
   */
  public List<Rect> targetsOrderedByCenterX() {
    targets.sort(Comparator.comparingInt(r -> r.center().x));
    return targets;
  }

  /**
   * Returns a {@code List} of target {@code Rect}s in {@code Rect.center().y} ascending order.
   *
   * @return a sorted {@code List}
   */
  public List<Rect> targetsOrderedByCenterY() {
    targets.sort(Comparator.comparingInt(r -> r.center().y));
    return targets;
  }

  /**
   * Returns a {@code List} of target {@code Rect}s in {@code Rect.bottomRight.y} ascending order.
   *
   * @return a sorted {@code List}
   */

  public List<Rect> targetsOrderedByBottomRightX() {
    targets.sort(Comparator.comparingInt(r -> r.bottomRight.x));
    return targets;
  }

  /**
   * Returns a {@code List} of target {@code Rect}s in {@code Rect.bottomRight.x} ascending order.
   *
   * @return a sorted {@code List}
   */
  public List<Rect> targetsOrderedByBottomRightY() {
    targets.sort(Comparator.comparingInt(r -> r.bottomRight.y));
    return targets;
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

  private static class JsonAdapterImpl implements DeadeyeJsonAdapter<TargetListTargetData> {

    // json d field: bb.tl().x, bb.tl().y, bb.br().x, bb.br().y, center.x, center.y
    private static final JsonReader.Options OPTIONS = JsonReader.Options.of("id", "sn", "v", "d");

    @Override
    public TargetListTargetData fromJson(BufferedSource source) throws IOException {
      JsonReader reader = JsonReader.of(source);
      String id = null;
      int serial = -1;
      boolean valid = false;
      List<Rect> targets = new ArrayList<>();

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
              targets.add(new Rect(topLeft, bottomRight, data[4]));
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
      for (Rect t : targetData.targets) {
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
