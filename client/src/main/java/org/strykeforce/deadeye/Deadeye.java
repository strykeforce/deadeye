package org.strykeforce.deadeye;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.util.regex.Pattern;

/**
 * Represents a connection to a Deadeye camera. It provides methods to configure and control the
 * camera and to receive target data.
 */
public class Deadeye {

    private final NetworkTable table;
    private final Link link;
    private final String id;


    /**
     * Initialize a connection to a Deadeye camera.
     *
     * @param id the camera id.
     */
    public Deadeye(String id) {
        this(id, NetworkTableInstance.getDefault());
    }

    /**
     * Initialize a connection to a Deadeye camera using a specified NetworkTables instance. This is
     * primarily used for testing.
     *
     * @param id                   the camera id.
     * @param networkTableInstance the NetworkTables instance
     */
    public Deadeye(String id, NetworkTableInstance networkTableInstance) {
        link = new Link(networkTableInstance);
        if (!Pattern.matches("^[A-Za-z][0-4]$", id))
            throw new IllegalArgumentException(id);
        this.id = id.toUpperCase();
        char unit = this.id.charAt(0);
        char inum = this.id.charAt(1);
        table = networkTableInstance.getTable(Link.DEADEYE_TABLE + "/" + unit + "/" + inum);
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
        if (!hasInfo()) return null;
        String info = table.getEntry("Info").getString("{}");
        return null;
    }

    public boolean hasInfo() {
        return table.containsKey("Info");
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
    }
}
