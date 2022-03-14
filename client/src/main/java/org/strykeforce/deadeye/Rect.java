package org.strykeforce.deadeye;

import java.util.Objects;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/**
 * A rectangle in (x, y) coordinate space, specified in integer precision.
 */
public class Rect {

  /**
   * The coordinates of the top left corner of this rectangle.
   */
  @NotNull
  public final Point topLeft;

  /**
   * The coordinates of the bottom right corner of this rectangle.
   */
  @NotNull
  public final Point bottomRight;

  /**
   * The contour area of the enclosed OpenCV contour.
   */
  public final int contourArea;

  /**
   * Constructs a rectangle with the specified (x, y) corner points.
   *
   * @param topLeft     the coordinates of the top left corner
   * @param bottomRight the coordinates of the bottom right corner
   * @param contourArea the area of the contour contained by this rectangle
   */
  public Rect(@NotNull Point topLeft, @NotNull Point bottomRight, int contourArea) {
    Objects.requireNonNull(topLeft);
    Objects.requireNonNull(bottomRight);
    this.topLeft = topLeft;
    this.bottomRight = bottomRight;
    this.contourArea = contourArea;
  }

  /**
   * Return the center of this rectangle.
   *
   * @return the center (x, y) point
   */
  public @NotNull Point center() {
    return new Point((topLeft.x + bottomRight.x) / 2, (topLeft.y + bottomRight.y) / 2);
  }

  /**
   * Return the height of this rectangle.
   *
   * @return the height in units of (x, y) coordinate system
   */
  public int height() {
    return bottomRight.y - topLeft.y;
  }

  /**
   * Return the width of this rectangle.
   *
   * @return the width in units of (x, y) coordinate system
   */
  public int width() {
    return bottomRight.x - topLeft.x;
  }

  /**
   * Return the size of this rectangle.
   *
   * @return the height x width in units of (x, y) coordinate system
   */
  public int size() {
    return height() * width();
  }

  /**
   * Checks whether this rectangle contains the point.
   *
   * <p>For example:
   *
   * <pre><code>
   *     assertTrue(rect.contains(rect.topLeft));
   *     assertFalse(rect.contains(rect.bottomRight));
   *     assertTrue(rect.contains(rect.center()));
   * </code></pre>
   *
   * @param point the (x, y) point to check
   * @return true if point is contained in rectangle
   */
  public boolean contains(@NotNull Point point) {
    return topLeft.x <= point.x && point.x < bottomRight.x && topLeft.y <= point.y
        && point.y < bottomRight.y;
  }

  /**
   * Returns true if rectangle is empty, i.e. height or width is less than or equal to zero.
   *
   * @return true if empty
   */
  public boolean isEmpty() {
    return height() <= 0 || width() <= 0;
  }

  @Override
  public boolean equals(@Nullable Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }
    Rect rect = (Rect) o;
    return contourArea == rect.contourArea && topLeft.equals(rect.topLeft) && bottomRight.equals(
        rect.bottomRight);
  }

  @Override
  public int hashCode() {
    return Objects.hash(topLeft, bottomRight, contourArea);
  }

  @Override
  public @NotNull String toString() {
    return "Rect{" + "topLeft=" + topLeft + ", bottomRight=" + bottomRight + ", contourArea="
        + contourArea + '}';
  }
}
