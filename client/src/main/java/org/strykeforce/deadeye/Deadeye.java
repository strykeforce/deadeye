package org.strykeforce.deadeye;

import com.squareup.moshi.JsonAdapter;
import com.squareup.moshi.Moshi;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.Objects;
import java.util.regex.Pattern;
import okio.BufferedSource;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Objects of the {@code Deadeye} class represent a connection to a Deadeye camera pipeline. It
 * provides methods to configure and control the camera and to receive target data.
 * <p>
 * To use this class make sure that the {@code TargetData} type that you construct this class with
 * is the same as the Deadeye pipeline target data type. These include:
 * <ul>
 *   <li>{@code UprightRectTargetData} when using an {@code UprightRectPipeline}</li>
 *   <li>{@code MinAreaRectTargetData} when using a {@code MinAreaRectPipeline}</li>
 *   <li>{@code TargetListTargetData} when using a {@code TargetListPipeline}</li>
 * </ul>
 *
 * <p>
 * For example, assuming our Deadeye camera "A0" uses an {@code UprightRectPipeline},
 * the constructor for this class is:
 *
 * <blockquote>
 * <pre>
 * Deadeye&#60;UprightRectTargetData&#62; deadeye = new Deadeye&#60;&#62;("A0", UprightRectTargetData.class);
 * </pre></blockquote>
 */
@SuppressWarnings("unused")
public class Deadeye<T extends TargetData> {

  static final Logger logger = LoggerFactory.getLogger(Deadeye.class);
  private static volatile Link link;
  private final NetworkTable table;
  private final String id;
  private final DeadeyeJsonAdapter<T> jsonAdapter;
  private TargetDataListener<T> targetDataListener;
  private T targetData;

  /**
   * Constructs an instance of {@code Deadeye} and initializes a connection to the associated
   * Deadeye camera.
   *
   * @param id  the camera id, i.e. the Deadeye unit letter followed by the camera number, for
   *            example "A0".
   * @param cls the {@code TargetData} Java class corresponding to the Deadeye pipeline type.
   * @throws NullPointerException     if the {@code cls} is {@code null}
   * @throws IllegalArgumentException if the {@code id} is not a letter followed by a 0-4 digit
   * @throws IllegalArgumentException if the {@code cls} is not a valid {@code TargetData} class
   */
  public Deadeye(String id, Class<T> cls) {
    this(id, cls, NetworkTableInstance.getDefault(), null);
  }

  /**
   * Constructs an instance of {@code Deadeye} and initializes a connection to the associated
   * Deadeye camera while manually specifying this client IP address.
   * <p>
   * Manually specifying the client IP address should only be done once, for the first {@code
   * Deadeye} instance initialized.
   * <p>
   *
   * @param id          the camera id, i.e. the Deadeye unit letter followed by the camera number,
   *                    for example "A0".
   * @param cls         the {@code TargetData} Java class corresponding to the Deadeye pipeline
   *                    type.
   * @param linkAddress the IP address the Deadeye daemon should send data to. If null, this client
   *                    will reuse a previously configured address or attempt to automatically
   *                    detect the correct IP address.
   * @throws NullPointerException     if the {@code cls} is {@code null}
   * @throws IllegalArgumentException if the {@code id} is not a letter followed by a 0-4 digit
   * @throws IllegalArgumentException if the {@code cls} is not a valid {@code TargetData} class
   * @throws IllegalArgumentException if the {@code linkAddress} supplied more than once and does
   *                                  not match address already in use
   */
  public Deadeye(String id, Class<T> cls, String linkAddress) {
    this(id, cls, NetworkTableInstance.getDefault(), linkAddress);
  }


  /**
   * Constructs an instance of {@code Deadeye} and initializes a connection to a Deadeye camera
   * using a specified NetworkTables instance. This constructor is primarily for testing or when
   * using a robot simulator.
   *
   * @param id  the camera id, i.e. the Deadeye unit letter followed by the camera number, for
   *            example "A0".
   * @param cls the {@code TargetData} Java class corresponding to the Deadeye pipeline type.
   * @param nti the NetworkTables instance to connect through.
   * @throws NullPointerException     if the {@code cls} or {@code nti} is {@code null}
   * @throws IllegalArgumentException if the {@code id} is not a letter followed by a 0-4 digit
   * @throws IllegalArgumentException if the {@code cls} is not a valid {@code TargetData} class
   */
  public Deadeye(String id, Class<T> cls, NetworkTableInstance nti) {
    this(id, cls, nti, null);
  }

  /**
   * Constructs an instance of {@code Deadeye} and initializes a connection to a Deadeye camera
   * using a specified NetworkTables instance while manually specifying this client IP address. This
   * constructor is primarily for testing or when using a robot simulator.
   * <p>
   * Manually specifying the client IP address should only be done once, for the first {@code
   * Deadeye} instance initialized.
   *
   * @param id          the camera id, i.e. the Deadeye unit letter followed by the camera number,
   *                    for example "A0".
   * @param cls         the {@code TargetData} Java class corresponding to the Deadeye pipeline
   *                    type.
   * @param nti         the NetworkTables instance to connect through.
   * @param linkAddress the IP address the Deadeye daemon should send data to. If null, this client
   *                    will attempt to automatically detect the correct IP address.
   * @throws NullPointerException     if the {@code cls} or {@code nti} is {@code null}
   * @throws IllegalArgumentException if the {@code id} is not a letter followed by a 0-4 digit
   * @throws IllegalArgumentException if the {@code cls} is not a valid {@code TargetData} class
   * @throws IllegalArgumentException if the {@code linkAddress} supplied more than once and does
   *                                  not match address already in use
   */
  @SuppressWarnings("unchecked")
  public Deadeye(String id, Class<T> cls, NetworkTableInstance nti, String linkAddress) {
    Objects.requireNonNull(cls, "cls must not be null");
    Objects.requireNonNull(nti, "nti must not be null");
    if (!Pattern.matches("^[A-Za-z][0-4]$", id)) {
      throw new IllegalArgumentException(id);
    }

    if (link == null) {
      synchronized (Link.class) {
        if (link == null) {
          link = new Link(nti, linkAddress);
        }
      }
    }

    if (linkAddress != null && !link.getAddress().equals(linkAddress)) {
      throw new IllegalArgumentException(
          "supplied linkAddress does not match address already in use");
    }

    link.addTargetDataHandler(id, this);
    this.id = id.toUpperCase();

    char unit = this.id.charAt(0);
    char inum = this.id.charAt(1);
    table = nti.getTable(Link.DEADEYE_TABLE + "/" + unit + "/" + inum);

    if (!link.isAlive()) {
      synchronized (Link.class) {
        if (!link.isAlive()) {
          link.start();
        }
      }
    }

    try {
      Constructor<T> constructor = cls.getConstructor();
      targetData = constructor.newInstance();
      jsonAdapter = targetData.getJsonAdapter();
    } catch (NoSuchMethodException
        | InstantiationException
        | IllegalAccessException
        | InvocationTargetException e) {
      logger.error("Unable to initialize target data", e);
      throw new IllegalArgumentException(e);
    }
  }

  static JsonAdapter<Info> getInfoJsonAdapter() {
    Moshi moshi = new Moshi.Builder().build();
    return moshi.adapter(Info.class);
  }

  /**
   * Gets the  {@code TargetDataListener} registered with this {@code Deadeye} instance.
   *
   * @return the associated {@code TargetDataListener}.
   */
  public TargetDataListener<T> getTargetDataListener() {
    return targetDataListener;
  }

  /**
   * Registers a {@code TargetDataListener} for this {@code Deadeye} instance that is called each
   * time {@code TargetData} is received.
   *
   * @param targetDataListener class to receive target data
   */
  public void setTargetDataListener(TargetDataListener<T> targetDataListener) {
    this.targetDataListener = targetDataListener;
  }

  /**
   * Processes a target data JSON payload received from the Deadeye pipeline into an appropriate
   * {@code TargetData} and calls the {@code TargetDataListener} assigned to this {@code Deadeye}
   * instance.
   *
   * @param source the JSON payload to process.
   * @throws IOException if the JSON payload can not be processed.
   */
  public void handleTargetData(BufferedSource source) throws IOException {
    targetData = jsonAdapter.fromJson(source);
    if (targetDataListener != null) {
      targetDataListener.onTargetData(targetData);
    }
  }

  /**
   * Gets the most recent {@code TargetData} received by this {@code Deadeye} instance.
   *
   * @return the last valid target data update.
   */
  public T getTargetData() {
    return targetData;
  }

  /**
   * Gets camera on or off state. This value may lag actual state if called immediately after
   * enabling or disabling camera.
   *
   * @return true if camera is on.
   */
  public boolean getEnabled() {
    return table.getEntry("On").getBoolean(false);
  }

  /**
   * Turns camera on or off.
   *
   * @param enabled true to turn camera on.
   */
  public void setEnabled(boolean enabled) {
    if (enabled) {
      table.getEntry("On").setBoolean(true);
    } else {
      table.getEntry("Off").setBoolean(true);
    }
  }

  /**
   * Gets camera light on or off state. This value may lag actual state if called immediately after
   * enabling or disabling camera or lights.
   *
   * @return true if light is on.
   */
  public boolean getLightEnabled() {
    NetworkTable light = table.getSubTable("Light");
    return light.getEntry("On").getBoolean(false);
  }

  /**
   * Turns camera light on or off.
   *
   * @param enabled true to turn light on.
   */
  public void setLightEnabled(boolean enabled) {
    NetworkTable light = table.getSubTable("Light");
    if (enabled) {
      light.getEntry("On").setBoolean(true);
    } else {
      light.getEntry("Off").setBoolean(true);
    }
  }

  /**
   * Gets the configured pipeline ID.
   *
   * @return the pipeline ID.
   */
  @NotNull
  public String getId() {
    return id;
  }

  /**
   * Gets information about the pipeline.
   *
   * @return the pipeline information.
   */
  @Nullable
  public Info getInfo() {
    String json = table.getEntry("Info").getString("");
    JsonAdapter<Info> jsonAdapter = getInfoJsonAdapter();
    Info info = null;
    try {
      info = jsonAdapter.fromJson(json);
    } catch (IOException e) {
      logger.error("error reading Info", e);
    }
    return info;
  }

  Link getLink() {
    return link;
  }

  /**
   * An {@code Info} represents Deadeye pipeline configuration information.
   */
  public static class Info {

    /**
     * True if logging is enabled for this pipeline.
     */
    public final boolean logging;
    /**
     * The type of pipeline, e.g. {@code UprightRectPipeline}.
     */
    public final String pipeline;
    /**
     * The version of the pipeline.
     */
    public final String version;

    public Info(boolean logging, String pipeline, String version) {
      this.logging = logging;
      this.pipeline = pipeline;
      this.version = version;
    }

    @Override
    public boolean equals(Object o) {
      if (this == o) {
        return true;
      }
      if (o == null || getClass() != o.getClass()) {
        return false;
      }
      Info info = (Info) o;
      return logging == info.logging
          && pipeline.equals(info.pipeline)
          && version.equals(info.version);
    }

    @Override
    public int hashCode() {
      return Objects.hash(logging, pipeline, version);
    }

    @Override
    public String toString() {
      return "Info{"
          + "logging="
          + logging
          + ", pipeline='"
          + pipeline
          + '\''
          + ", version='"
          + version
          + '\''
          + '}';
    }
  }
}
