package org.strykeforce.deadeye;

import java.util.Objects;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/**
 * A point representing a location in (x,y) coordinate space, specified in double precision.
 */
@SuppressWarnings("unused")
public final class Point2D {

  // CHECKSTYLE:OFF
  public final double x;
  public final double y;
  // CHECKSTYLE:ON

  /**
   * Constructs and initializes a point at the specified (x,y) location in the coordinate space.
   *
   * @param x the X coordinate of the newly constructed {@code Point2D}
   * @param y the y coordinate of the newly constructed {@code Point2D}
   */
  public Point2D(double x, double y) {
    this.x = x;
    this.y = y;
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
    Point2D point = (Point2D) o;
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
