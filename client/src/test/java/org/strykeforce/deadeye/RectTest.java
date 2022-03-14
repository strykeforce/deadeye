package org.strykeforce.deadeye;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotSame;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.jetbrains.annotations.NotNull;
import org.junit.jupiter.api.Test;

class RectTest {

  @NotNull Rect rect = new Rect(new Point(0, 0), new Point(10, 20), 10);

  @Test
  void center() {
    assertEquals(new Point(5, 10), rect.center());
  }

  @Test
  void height() {
    assertEquals(20, rect.height());
  }

  @Test
  void width() {
    assertEquals(10, rect.width());
  }

  @Test
  void size() {
    assertEquals(200, rect.size());
  }

  @Test
  void contains() {
    assertTrue(rect.contains(rect.topLeft));
    assertFalse(rect.contains(rect.bottomRight));
    assertTrue(rect.contains(rect.center()));
  }

  @Test
  void isEmpty() {
    assertFalse(rect.isEmpty());
    assertTrue(new Rect(rect.bottomRight, rect.topLeft, 0).isEmpty());
    assertTrue(new Rect(new Point(10,10), new Point(10,20), 0).isEmpty());
    assertTrue(new Rect(new Point(10,10), new Point(9,20), 0).isEmpty());
    assertTrue(new Rect(new Point(0,10), new Point(10,10), 0).isEmpty());
    assertTrue(new Rect(new Point(0,10), new Point(10,9), 0).isEmpty());
  }

  @Test
  void equality() {
    Rect expected = new Rect(new Point(0, 0), new Point(10, 20), 10);
    assertNotSame(expected, rect);
    assertEquals(expected, rect);
  }

  @Test
  void hashCodeEquality() {
    Rect expected = new Rect(new Point(0, 0), new Point(10, 20), 10);
    assertEquals(expected.hashCode(), rect.hashCode());
  }
}