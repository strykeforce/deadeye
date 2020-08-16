package org.strykeforce.deadeye;

@FunctionalInterface
public interface TargetDataListener<T extends TargetData> {
    void onTargetData(T data);
}
