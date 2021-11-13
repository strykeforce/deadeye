package org.strykeforce.deadeye;

/**
 * A class implements {@code TargetDataListener} to receive updates of the specifed {@code
 * TargetData} from a Deadeye camera pipeline.
 *
 * @param <T> the {@code TargetData} type this listener should receive
 */
@FunctionalInterface
public interface TargetDataListener<T extends TargetData> {

  /**
   * Called when {@code TargetData} is received by the Deadeye camera pipeline.
   *
   * @param data the received {@code TargetData}
   */
  void onTargetData(T data);
}
