package org.strykeforce.deadeye;

import java.util.Objects;

public final class Point2D {
  public final double x;
  public final double y;

  public Point2D(double x, double y) {
    this.x = x;
    this.y = y;
  }

  public double r() {
    return Math.sqrt(x * x + y * y);
  }

  public double theta() {
    return Math.atan2(y, x);
  }

  public double distanceTo(Point that) {
    double dx = this.x - that.x;
    double dy = this.y - that.y;
    return Math.sqrt(dx * dx + dy * dy);
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (o == null || getClass() != o.getClass()) return false;
    Point2D point = (Point2D) o;
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
