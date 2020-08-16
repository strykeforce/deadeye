package org.strykeforce.deadeye;

import com.squareup.moshi.Moshi;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import okio.BufferedSource;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.util.Objects;
import java.util.regex.Pattern;

/**
 * Represents a connection to a Deadeye camera. It provides methods to configure and control the
 * camera and to receive target data.
 */
@SuppressWarnings("unused")
public class Deadeye<T extends TargetData> implements TargetDataHandler {

    static final Logger logger = LoggerFactory.getLogger(Deadeye.class);
    private static Link link;
    private final NetworkTable table;
    private final String id;
    private final org.strykeforce.deadeye.JsonAdapter<T> jsonAdapter;
    private TargetDataListener<T> targetDataListener;
    private T targetData;

    /**
     * Initialize a connection to a Deadeye camera.
     *
     * @param id          the camera id.
     * @param jsonAdapter
     */
    public Deadeye(String id, org.strykeforce.deadeye.JsonAdapter<T> jsonAdapter) {
        this(id, jsonAdapter, NetworkTableInstance.getDefault());
    }

    /**
     * Initialize a connection to a Deadeye camera using a specified NetworkTables instance. This is
     * primarily used for testing.
     *
     * @param id                   the camera id.
     * @param jsonAdapter          JsonAdaptor to convert JSON to/from TargetData type
     * @param nti the NetworkTables instance
     */
    public Deadeye(String id, org.strykeforce.deadeye.JsonAdapter<T> jsonAdapter, NetworkTableInstance nti) {
        if (!Pattern.matches("^[A-Za-z][0-4]$", id)) {
            throw new IllegalArgumentException(id);
        }

        if (link == null) {
            synchronized (Link.class) {
                if (link == null)
                    link = new Link(nti);
            }
        }

        link.addTargetDataHandler(id, this);
        this.jsonAdapter = jsonAdapter;
        this.id = id.toUpperCase();

        char unit = this.id.charAt(0);
        char inum = this.id.charAt(1);
        table = nti.getTable(Link.DEADEYE_TABLE + "/" + unit + "/" + inum);

        if (!link.isAlive()) {
            synchronized (Link.class) {
                if (!link.isAlive())
                    link.start();
            }
        }
    }

    /**
     * Call this listener with target data each time update packets are received from Deadeye daemon.
     *
     * @param targetDataListener class to receive target data
     */
    public void setTargetDataListener(TargetDataListener<T> targetDataListener) {
        this.targetDataListener = targetDataListener;
    }

    /**
     * Get TargetDataListener.
     * @return the TargetDataListener.
     */
    public TargetDataListener<T> getTargetDataListener() {
        return targetDataListener;
    }

    @Override
    public void handleTargetData(BufferedSource source) throws IOException {
        targetData = jsonAdapter.fromJson(source);
        if (targetDataListener != null)
            targetDataListener.onTargetData(targetData);
    }


    /**
     * Get the most recent target data.
     *
     * @return the last valid target data update.
     */
    public T getTargetData() {
        return targetData;
    }

    /**
     * Get camera on or off state. This value may lag actual state if called immediately after
     * enabling or disabling camera.
     *
     * @return true if camera is on.
     */
    public boolean getEnabled() {
        return table.getEntry("On").getBoolean(false);
    }

    /**
     * Turn camera on or off.
     *
     * @param enabled true to turn camera on.
     */
    public void setEnabled(boolean enabled) {
        if (enabled)
            table.getEntry("On").setBoolean(true);
        else
            table.getEntry("Off").setBoolean(true);
    }

    /**
     * Get camera light on or off state. This value may lag actual state if called immediately after
     * enabling or disabling camera or lights.
     *
     * @return true if light is on.
     */
    public boolean getLightEnabled() {
        NetworkTable light = table.getSubTable("Light");
        return light.getEntry("On").getBoolean(false);
    }

    /**
     * Turn camera light on or off.
     *
     * @param enabled true to turn light on.
     */
    public void setLightEnabled(boolean enabled) {
        NetworkTable light = table.getSubTable("Light");
        if (enabled)
            light.getEntry("On").setBoolean(true);
        else
            light.getEntry("Off").setBoolean(true);
    }

    @NotNull
    public String getId() {
        return id;
    }

    @Nullable
    public Info getInfo() {
        String json = table.getEntry("Info").getString("");
        com.squareup.moshi.JsonAdapter<Info> jsonAdapter = getInfoJsonAdapter();
        Info info = null;
        try {
            info = jsonAdapter.fromJson(json);
        } catch (IOException e) {
            logger.error("error reading Info", e);
        }
        return info;
    }

    static com.squareup.moshi.JsonAdapter<Info> getInfoJsonAdapter() {
        Moshi moshi = new Moshi.Builder().build();
        return moshi.adapter(Info.class);
    }

    Link getLink() {
        return link;
    }

    static class Info {
        private final boolean logging;
        private final String pipeline;
        private final String version;

        public Info(boolean logging, String pipeline, String version) {
            this.logging = logging;
            this.pipeline = pipeline;
            this.version = version;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            Info info = (Info) o;
            return logging == info.logging &&
                    pipeline.equals(info.pipeline) &&
                    version.equals(info.version);
        }

        @Override
        public int hashCode() {
            return Objects.hash(logging, pipeline, version);
        }

        @Override
        public String toString() {
            return "Info{" +
                    "logging=" + logging +
                    ", pipeline='" + pipeline + '\'' +
                    ", version='" + version + '\'' +
                    '}';
        }
    }
}
