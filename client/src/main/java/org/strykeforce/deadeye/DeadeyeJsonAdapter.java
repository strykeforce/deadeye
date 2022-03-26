package org.strykeforce.deadeye;

import java.io.IOException;
import okio.BufferedSource;
import org.jetbrains.annotations.NotNull;

/**
 * A {@code TargetData} class implements {@code DeadeyeJsonAdapter} to enable serialization and
 * deserialization from JSON.
 *
 * @param <T> the {@code TargetData} type to deserialize.
 */
public interface DeadeyeJsonAdapter<T extends TargetData> {

  /**
   * Deserializes this {@code TargetData} from JSON.
   *
   * @param source contains the JSON to deserialize.
   * @return a new {@code TargetData} object, initialized with supplied JSON.
   * @throws IOException if a deserialization error occurs.
   */
  @NotNull T fromJson(@NotNull BufferedSource source) throws IOException;

  /**
   * Serializes this {@code TargetData} to JSON.
   *
   * @param targetData the object to serialize.
   * @return this object in JSON format.
   * @throws IOException if a serialization error occurs.
   */
  @NotNull String toJson(@NotNull T targetData) throws IOException;
}
