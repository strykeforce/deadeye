package org.strykeforce.deadeye;

import java.util.Objects;

public class UprightRectTargetData extends TargetData {

    static final int DATA_LENGTH = 6;

    final Point topLeft;
    final Point bottomRight;
    final Point center;

    public UprightRectTargetData(String id, int serial, boolean valid, Point topLeft, Point bottomRight, Point center) {
        super(id, serial, valid);
        this.topLeft = topLeft;
        this.bottomRight = bottomRight;
        this.center = center;
    }


    public int area() {
        return width() * height();
    }

    public int width() {
        return bottomRight.x - topLeft.x;
    }

    public int height() {
        return bottomRight.y - topLeft.y;
    }

    @Override
    public boolean equals(Object o) {
        if (this==o) return true;
        if (o==null || getClass()!=o.getClass()) return false;
        if (!super.equals(o)) return false;
        UprightRectTargetData that = (UprightRectTargetData) o;
        return topLeft.equals(that.topLeft) &&
                bottomRight.equals(that.bottomRight) &&
                center.equals(that.center);
    }

    @Override
    public int hashCode() {
        return Objects.hash(super.hashCode(), topLeft, bottomRight, center);
    }

    @Override
    public String toString() {
        return "UprightRectTargetData{" +
                "topLeft=" + topLeft +
                ", bottomRight=" + bottomRight +
                ", offset=" + center +
                "} " + super.toString();
    }
}
