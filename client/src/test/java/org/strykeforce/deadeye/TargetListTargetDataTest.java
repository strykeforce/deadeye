package org.strykeforce.deadeye;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.Arrays;
import org.junit.jupiter.api.Test;

class TargetListTargetDataTest {


  @Test
  void targetsOrderedByTopLeftX() {
    Rect r1 = new Rect(new Point(0, 0), new Point(10, 10), 10);
    Rect r2 = new Rect(new Point(5, 0), new Point(15, 10), 10);
    Rect r3 = new Rect(new Point(10, 0), new Point(20, 10), 10);

    var targets = Arrays.asList(r2, r1, r3);
    TargetListTargetData td = new TargetListTargetData("Z0", 0, true, targets);
    var expected = Arrays.asList(r1, r2, r3);
    assertEquals(expected, td.targetsOrderedByTopLeftX());

    targets = Arrays.asList(r2, r1, r3, r2);
    td = new TargetListTargetData("Z0", 0, true, targets);
    expected = Arrays.asList(r1, r2, r2, r3);
    assertEquals(expected, td.targetsOrderedByTopLeftX());
  }

  @Test
  void targetsOrderedByTopLeftY() {
    Rect r1 = new Rect(new Point(0, 5), new Point(10, 10), 10);
    Rect r2 = new Rect(new Point(0, 10), new Point(10, 15), 10);
    Rect r3 = new Rect(new Point(0, 15), new Point(10, 20), 10);

    var targets = Arrays.asList(r2, r1, r3);
    TargetListTargetData td = new TargetListTargetData("Z0", 0, true, targets);
    var expected = Arrays.asList(r1, r2, r3);
    assertEquals(expected, td.targetsOrderedByTopLeftY());

    targets = Arrays.asList(r2, r1, r3, r2);
    td = new TargetListTargetData("Z0", 0, true, targets);
    expected = Arrays.asList(r1, r2, r2, r3);
    assertEquals(expected, td.targetsOrderedByTopLeftY());
  }

  @Test
  void targetsOrderedByCenterX() {
    Rect r1 = new Rect(new Point(0, 5), new Point(10, 10), 10);
    Rect r2 = new Rect(new Point(0, 10), new Point(20, 15), 10);
    Rect r3 = new Rect(new Point(0, 15), new Point(30, 20), 10);

    var targets = Arrays.asList(r2, r1, r3);
    TargetListTargetData td = new TargetListTargetData("Z0", 0, true, targets);
    var expected = Arrays.asList(r1, r2, r3);
    assertEquals(expected, td.targetsOrderedByCenterX());
  }

  @Test
  void targetsOrderedByCenterY() {
    Rect r1 = new Rect(new Point(0, 5), new Point(10, 15), 10);
    Rect r2 = new Rect(new Point(0, 10), new Point(20, 25), 10);
    Rect r3 = new Rect(new Point(0, 15), new Point(30, 35), 10);

    var targets = Arrays.asList(r2, r1, r3);
    TargetListTargetData td = new TargetListTargetData("Z0", 0, true, targets);
    var expected = Arrays.asList(r1, r2, r3);
    assertEquals(expected, td.targetsOrderedByCenterY());
  }

  @Test
  void targetsOrderedByBottomRightX() {
    Rect r1 = new Rect(new Point(0, 5), new Point(10, 15), 10);
    Rect r2 = new Rect(new Point(0, 10), new Point(20, 15), 10);
    Rect r3 = new Rect(new Point(0, 15), new Point(30, 15), 10);

    var targets = Arrays.asList(r2, r1, r3);
    TargetListTargetData td = new TargetListTargetData("Z0", 0, true, targets);
    var expected = Arrays.asList(r1, r2, r3);
    assertEquals(expected, td.targetsOrderedByCenterY());
  }

  @Test
  void targetsOrderedByBottomRightY() {
    Rect r1 = new Rect(new Point(0, 5), new Point(10, 15), 10);
    Rect r2 = new Rect(new Point(0, 10), new Point(20, 25), 10);
    Rect r3 = new Rect(new Point(0, 15), new Point(30, 35), 10);

    var targets = Arrays.asList(r2, r1, r3);
    TargetListTargetData td = new TargetListTargetData("Z0", 0, true, targets);
    var expected = Arrays.asList(r1, r2, r3);
    assertEquals(expected, td.targetsOrderedByBottomRightY());
  }
}