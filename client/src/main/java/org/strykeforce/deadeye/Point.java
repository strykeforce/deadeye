package org.strykeforce.deadeye;

import java.util.Objects;

public final class Point {
  public final int x;
  public final int y;

  public Point(int x, int y) {
    this.x = x;
    this.y = y;
  }

  /** Return the center of a bounding box formed by top left and bottom right points. */
  public static Point boundingBoxCenterFrom(Point topLeft, Point bottomRight) {
    return new Point((topLeft.x + bottomRight.x) / 2, (topLeft.y + bottomRight.y) / 2);
  }

  /** Polar coordinate radius of this point. */
  public double r() {
    return Math.sqrt(x * x + y * y);
  }

  /** Polar coordinate angle of this point. */
  public double theta() {
    return Math.atan2(y, x);
  }

  /** Distance to another point. */
  public double distanceTo(Point that) {
    double dx = this.x - that.x;
    double dy = this.y - that.y;
    return Math.sqrt(dx * dx + dy * dy);
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (o == null || getClass() != o.getClass()) return false;
    Point point = (Point) o;
    return x == point.x && y == point.y;
  }

  @Override
  public int hashCode() {
    return Objects.hash(x, y);
  }

  @Override
  public String toString() {
    return "(" + x + ", " + y + ")";
  }
}
