package org.strykeforce.deadeye;

import com.squareup.moshi.Json;
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
 *
 * <p>To use this class make sure that the {@code TargetData} type that you construct this class
 * with is the same as the Deadeye pipeline target data type. These include:
 * <ul>
 *   <li>{@code UprightRectTargetData} when using an {@code UprightRectPipeline}</li>
 *   <li>{@code MinAreaRectTargetData} when using a {@code MinAreaRectPipeline}</li>
 *   <li>{@code TargetListTargetData} when using a {@code TargetListPipeline}</li>
 * </ul>
 *
 * <p>For example, assuming our Deadeye camera "A0" uses an {@code UprightRectPipeline},
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
  private final @NotNull NetworkTable cameraTable;
  private static volatile @Nullable Link link;
  private final @NotNull String id;
  private final @NotNull DeadeyeJsonAdapter<T> jsonAdapter;
  private @NotNull TargetDataListener<T> targetDataListener = data -> {
  };
  private @NotNull T targetData;
  @SuppressWarnings("FieldCanBeLocal")
  private final @NotNull PipelineLogger pipelineLogger;

  /**
   * Constructs an instance of {@code Deadeye} and initializes a connection to the associated
   * Deadeye camera.
   *
   * @param id  the camera id, i.e. the Deadeye unit letter followed by the camera number, for
   *            example "A0".
   * @param cls the {@code TargetData} Java class corresponding to the Deadeye pipeline type.
   * @throws IllegalArgumentException if the {@code id} is not a letter followed by a 0-4 digit
   * @throws IllegalArgumentException if the {@code cls} is not a valid {@code TargetData} class
   */
  public Deadeye(@NotNull String id, @NotNull Class<T> cls) {
    this(id, cls, NetworkTableInstance.getDefault(), null);
  }

  /**
   * Constructs an instance of {@code Deadeye} and initializes a connection to the associated
   * Deadeye camera while manually specifying this client IP address.
   *
   * <p>Manually specifying the client IP address should only be done once, for the first {@code
   * Deadeye} instance initialized.
   *
   * @param id          the camera id, i.e. the Deadeye unit letter followed by the camera number,
   *                    for example "A0".
   * @param cls         the {@code TargetData} Java class corresponding to the Deadeye pipeline
   *                    type.
   * @param linkAddress the IP address the Deadeye daemon should send data to. If null, this client
   *                    will reuse a previously configured address or attempt to automatically
   *                    detect the correct IP address.
   * @throws IllegalArgumentException if the {@code id} is not a letter followed by a 0-4 digit
   * @throws IllegalArgumentException if the {@code cls} is not a valid {@code TargetData} class
   * @throws IllegalArgumentException if the {@code linkAddress} supplied more than once and does
   *                                  not match address already in use
   */
  public Deadeye(@NotNull String id, @NotNull Class<T> cls, @Nullable String linkAddress) {
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
   * @throws IllegalArgumentException if the {@code id} is not a letter followed by a 0-4 digit
   * @throws IllegalArgumentException if the {@code cls} is not a valid {@code TargetData} class
   */
  public Deadeye(@NotNull String id, @NotNull Class<T> cls, @NotNull NetworkTableInstance nti) {
    this(id, cls, nti, null);
  }

  /**
   * Constructs an instance of {@code Deadeye} and initializes a connection to a Deadeye camera
   * using a specified NetworkTables instance while manually specifying this client IP address. This
   * constructor is primarily for testing or when using a robot simulator.
   *
   * <p>Manually specifying the client IP address should only be done once, for the first {@code
   * Deadeye} instance initialized.
   *
   * @param id          the camera id, i.e. the Deadeye unit letter followed by the camera number,
   *                    for example "A0".
   * @param cls         the {@code TargetData} Java class corresponding to the Deadeye pipeline
   *                    type.
   * @param nti         the NetworkTables instance to connect through.
   * @param linkAddress the IP address the Deadeye daemon should send data to. If null, this client
   *                    will attempt to automatically detect the correct IP address.
   * @throws IllegalArgumentException if the {@code id} is not a letter followed by a 0-4 digit
   * @throws IllegalArgumentException if the {@code cls} is not a valid {@code TargetData} class
   * @throws IllegalArgumentException if the {@code linkAddress} supplied more than once and does
   *                                  not match address already in use
   */
  @SuppressWarnings("unchecked")
  public Deadeye(
      @NotNull String id, @NotNull Class<T> cls, @NotNull NetworkTableInstance nti,
      @Nullable String linkAddress) {
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

    Link link = Objects.requireNonNull(Deadeye.link);
    if (linkAddress != null && !link.getAddress().equals(linkAddress)) {
      throw new IllegalArgumentException(
          "supplied linkAddress does not match address already in use");
    }

    link.addTargetDataHandler(id, this);
    this.id = id.toUpperCase();

    char unit = this.id.charAt(0);
    char inum = this.id.charAt(1);
    cameraTable = nti.getTable(Link.DEADEYE_TABLE + "/" + unit + "/" + inum);

    pipelineLogger = new PipelineLogger(cameraTable);

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

  static JsonAdapter<Capture> getCaptureJsonAdapter() {
    Moshi moshi = new Moshi.Builder().build();
    return moshi.adapter(Capture.class);
  }

  /**
   * Gets the  {@code TargetDataListener} registered with this {@code Deadeye} instance.
   *
   * @return the associated {@code TargetDataListener}.
   */
  @NotNull
  public TargetDataListener<T> getTargetDataListener() {
    return targetDataListener;
  }

  /**
   * Registers a {@code TargetDataListener} for this {@code Deadeye} instance that is called each
   * time {@code TargetData} is received.
   *
   * @param targetDataListener class to receive target data
   */
  public void setTargetDataListener(@NotNull TargetDataListener<T> targetDataListener) {
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
    targetDataListener.onTargetData(targetData);
  }

  /**
   * Gets the most recent {@code TargetData} received by this {@code Deadeye} instance.
   *
   * @return the last valid target data update.
   */
  @NotNull
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
    return cameraTable.getEntry("On").getBoolean(false);
  }

  /**
   * Turns camera on or off.
   *
   * @param enabled true to turn camera on.
   */
  public void setEnabled(boolean enabled) {
    if (enabled) {
      cameraTable.getEntry("On").setBoolean(true);
    } else {
      cameraTable.getEntry("Off").setBoolean(true);
    }
  }

  /**
   * Gets camera light on or off state. This value may lag actual state if called immediately after
   * enabling or disabling camera or lights.
   *
   * @return true if light is on.
   */
  public boolean getLightEnabled() {
    NetworkTable light = cameraTable.getSubTable("Light");
    return light.getEntry("On").getBoolean(false);
  }

  /**
   * Turns camera light on or off.
   *
   * @param enabled true to turn light on.
   */
  public void setLightEnabled(boolean enabled) {
    NetworkTable light = cameraTable.getSubTable("Light");
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
  @NotNull
  public Info getInfo() {
    String json = cameraTable.getEntry("Info").getString("");
    JsonAdapter<Info> jsonAdapter = getInfoJsonAdapter();
    try {
      return Objects.requireNonNull(jsonAdapter.fromJson(json));
    } catch (Exception e) {
      logger.error("error reading Info", e);
    }
    // return error Info
    return new Info();
  }

  /**
   * Gets the capture configuration for the pipeline.
   *
   * @return the capture configuration
   */
  @NotNull
  public Capture getCapture() {
    String json = cameraTable.getEntry("Capture").getString("");
    JsonAdapter<Capture> jsonAdapter = getCaptureJsonAdapter();
    try {
      return Objects.requireNonNull(jsonAdapter.fromJson(json));
    } catch (Exception e) {
      logger.error("error reading Capture", e);
    }
    // return error Capture
    return new Capture();
  }

  /**
   * Provide Link for testing.
   *
   * @return the {@code Link} or {@code null} if a Deadeye unit has not been initialized yet
   */
  @Nullable Link getLink() {
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

    public Info() {
      this(false, "ERROR", "ERROR");
    }

    /**
     * Create a new Info object.
     *
     * @param logging  true if logging enabled
     * @param pipeline the type of pipeline
     * @param version  the version of the pipeline
     */
    public Info(boolean logging, String pipeline, String version) {
      this.logging = logging;
      this.pipeline = pipeline;
      this.version = version;
    }

    @Override
    public boolean equals(@Nullable Object o) {
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
    public @NotNull String toString() {
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

  /**
   * An {@code Capture} represents Deadeye capture configuration information.
   */
  public static class Capture {

    /**
     * The capture type, e.g. "jetson".
     */
    public final String type;

    /**
     * The capture frame rate in frames per second.
     */
    @Json(name = "fps")
    public final int frameRate;

    /**
     * The width of the captured frame in pixels.
     */
    @Json(name = "w")
    public final int width;

    /**
     * The height of the captured frame in pixels.
     */
    @Json(name = "h")
    public final int height;

    public Capture() {
      this("ERROR", 0, 0, 0);
    }

    /**
     * Create a new Capture object.
     *
     * @param type      the capture type
     * @param frameRate the frame rate in FPS
     * @param width     the width of the captured frame
     * @param height    the height of the captured frame
     */
    public Capture(String type, int frameRate, int width, int height) {
      this.type = type;
      this.frameRate = frameRate;
      this.width = width;
      this.height = height;
    }

    @Override
    public boolean equals(@Nullable Object o) {
      if (this == o) {
        return true;
      }
      if (o == null || getClass() != o.getClass()) {
        return false;
      }
      Capture capture = (Capture) o;
      return frameRate == capture.frameRate && width == capture.width && height == capture.height
          && type.equals(capture.type);
    }

    @Override
    public int hashCode() {
      return Objects.hash(type, frameRate, width, height);
    }

    @Override
    public @NotNull String toString() {
      return "Capture{type='" + type + '\'' + ", frameRate=" + frameRate + ", width=" + width
          + ", height=" + height + '}';
    }
  }

}
