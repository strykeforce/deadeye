package org.strykeforce.deadeye;

import okio.BufferedSource;

import java.io.IOException;


public interface JsonAdapter<T extends TargetData> {
    T fromJson(BufferedSource source) throws IOException;
    String toJson(T targetData) throws IOException;
}
