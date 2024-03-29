package org.strykeforce.deadeye;

import java.util.Objects;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/**
 * A point representing a location in (x,y) coordinate space, specified in integer precision.
 */
@SuppressWarnings("unused")
public final class Point {

  // CHECKSTYLE:OFF
  /**
   * the X coordinate of this {@code Point}.
   */
  public final int x;
  /**
   * the Y coordinate of this {@code Point}.
   */
  public final int y;
  // CHECKSTYLE:ON

  /**
   * Constructs and initializes a point at the specified (x,y) location in the coordinate space.
   *
   * @param x the X coordinate of the newly constructed {@code Point}
   * @param y the y coordinate of the newly constructed {@code Point}
   */
  public Point(int x, int y) {
    this.x = x;
    this.y = y;
  }

  /**
   * Return the center of a bounding box specified by top left and bottom right corners.
   *
   * @param topLeft     the top left corner of the bounding box.
   * @param bottomRight the bottom right corner of the bounding box.
   * @return the {@code Point} at the center of this bounding box.
   */
  public static @NotNull Point boundingBoxCenterFrom(@NotNull Point topLeft, @NotNull Point bottomRight) {
    // Rect constructor checks for nulls
    return new Rect(topLeft, bottomRight, 0).center();
  }

  /**
   * Polar coordinate radius of this {@code Point}.
   *
   * @return the length of the radius from the origin to this {@code Point}.
   */
  public double radius() {
    return Math.sqrt(x * x + y * y);
  }

  /**
   * Polar coordinate angle of this {@code Point}.
   *
   * @return the angle from the x-axis to this {@code Point}.
   */
  public double theta() {
    return Math.atan2(y, x);
  }

  /**
   * Get the distance from this {@code Point} to another {@code Point}.
   *
   * @param that the point to measure distance from.
   * @return the distance to the other {@code Point}.
   */
  public double distanceTo(@NotNull Point that) {
    double dx = this.x - that.x;
    double dy = this.y - that.y;
    return Math.sqrt(dx * dx + dy * dy);
  }

  @Override
  public boolean equals(@Nullable Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }
    Point point = (Point) o;
    return x == point.x && y == point.y;
  }

  @Override
  public int hashCode() {
    return Objects.hash(x, y);
  }

  @Override
  public @NotNull String toString() {
    return "(" + x + ", " + y + ")";
  }
}
