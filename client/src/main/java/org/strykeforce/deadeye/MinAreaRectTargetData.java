package org.strykeforce.deadeye;

import com.squareup.moshi.JsonReader;
import com.squareup.moshi.JsonWriter;
import okio.Buffer;
import okio.BufferedSource;

import java.io.IOException;
import java.util.Arrays;
import java.util.Objects;

public class MinAreaRectTargetData extends TargetData {

  static final int DATA_LENGTH = 13;

  /** Angle of OpenCV rotated rectangle. */
  public final double angle;
  /** Center of OpenCV rotated rectangle. */
  public final Point2D center;
  /** Width of OpenCV rotated rectangle. */
  public final double width;
  /** Height of OpenCV rotated rectangle. */
  public final double height;
  /** Vertices of OpenCV rotated rectangle. */
  public final Point2D[] points;

  public MinAreaRectTargetData() {
    this("", 0, false, 0, new Point2D(0, 0), 0, 0, new Point2D[0]);
  }

  public MinAreaRectTargetData(
      String id,
      int serial,
      boolean valid,
      double angle,
      Point2D center,
      double width,
      double height,
      Point2D[] points) {
    super(id, serial, valid);
    this.angle = angle;
    this.center = center;
    this.width = width;
    this.height = height;
    this.points = points;
  }

  public double area() {
    return width * height;
  }

  @SuppressWarnings("rawtypes")
  @Override
  public DeadeyeJsonAdapter getJsonAdapter() {
    return new JsonAdapterImpl();
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (o == null || getClass() != o.getClass()) return false;
    if (!super.equals(o)) return false;
    MinAreaRectTargetData that = (MinAreaRectTargetData) o;
    return Double.compare(that.angle, angle) == 0
        && Double.compare(that.width, width) == 0
        && Double.compare(that.height, height) == 0
        && center.equals(that.center)
        && Arrays.equals(points, that.points);
  }

  @Override
  public int hashCode() {
    int result = Objects.hash(super.hashCode(), angle, center, width, height);
    result = 31 * result + Arrays.hashCode(points);
    return result;
  }

  @Override
  public String toString() {
    return "MinAreaRectTargetData{"
        + "angle="
        + angle
        + ", center="
        + center
        + ", width="
        + width
        + ", height="
        + height
        + ", points="
        + Arrays.toString(points)
        + "} "
        + super.toString();
  }

  private static class JsonAdapterImpl implements DeadeyeJsonAdapter<MinAreaRectTargetData> {

    // json d field:
    //   rect.angle,
    //   rect.center.x,
    //   rect.center.y,
    //   rect.size.height,
    //   rect.size.width,
    //   corners[0].x,
    //   corners[0].y,
    //   corners[1].x,
    //   corners[1].y,
    //   corners[2].x,
    //   corners[2].y,
    //   corners[3].x,
    //   corners[3].y,
    private static final JsonReader.Options OPTIONS = JsonReader.Options.of("id", "sn", "v", "d");

    @Override
    public MinAreaRectTargetData fromJson(BufferedSource source) throws IOException {
      JsonReader reader = JsonReader.of(source);
      String id = null;
      int serial = -1;
      boolean valid = false;
      double[] data = new double[DATA_LENGTH];
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
              data[i] = reader.nextDouble();
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
      Point2D center = new Point2D(data[1], data[2]);
      Point2D[] points =
          new Point2D[] {
            new Point2D(data[5], data[6]),
            new Point2D(data[7], data[8]),
            new Point2D(data[9], data[10]),
            new Point2D(data[11], data[12]),
          };
      return new MinAreaRectTargetData(
          id, serial, valid, data[0], center, data[4], data[3], points);
    }

    @Override
    public String toJson(MinAreaRectTargetData targetData) throws IOException {
      Buffer buffer = new Buffer();
      JsonWriter writer = JsonWriter.of(buffer);
      writer.beginObject();
      writer.name("id").value(targetData.id);
      writer.name("sn").value(targetData.serial);
      writer.name("v").value(targetData.valid);
      writer.name("d").beginArray();
      writer.value(targetData.angle);
      writer.value(targetData.center.x).value(targetData.center.y);
      writer.value(targetData.height).value(targetData.width);
      for (int i = 0; i < 4; i++) {
        writer.value(targetData.points[i].x).value(targetData.points[i].y);
      }
      writer.endArray();
      writer.endObject();
      return buffer.readUtf8();
    }
  }
}
