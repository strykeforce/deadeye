package org.strykeforce.deadeye;

import com.squareup.moshi.JsonReader;
import com.squareup.moshi.JsonWriter;
import okio.Buffer;
import okio.BufferedSource;

import java.io.IOException;
import java.util.Objects;

public class TargetData {


    public final String id;
    public final int serial;
    public final boolean valid;

    public TargetData(String id, int serial, boolean valid) {
        this.id = id;
        this.serial = serial;
        this.valid = valid;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        TargetData that = (TargetData) o;
        return serial == that.serial &&
                valid == that.valid &&
                id.equals(that.id);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, serial, valid);
    }

    @Override
    public String toString() {
        return "TargetData{" +
                "id='" + id + '\'' +
                ", serial=" + serial +
                ", valid=" + valid +
                '}';
    }
}
