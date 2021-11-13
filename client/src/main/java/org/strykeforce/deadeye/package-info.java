/**
 * Provides a Java client library for the Stryke Force FRC <a href="https://github.com/strykeforce/deadeye">Deadeye
 * Vision System</a>. The most important class is {@code Deadeye}, which represents a connected
 * Deadeye camera pipeline and is the starting point for using this library. A client (robot) can
 * receive target data updates by implementing the {@code TargetDataListener} interface and
 * registering it with a {@code Deadeye} instance.
 *
 * <p>
 * Classes {@code UprightRectTargetData}, {@code MinAreaRectTargetData}, and {@code
 * TargetListTargetData} are interfaces to the default Deadeye camera pipelines. Custom pipelines
 * can be created by subclassing {@code TargetData} and providing a corresponding {@code
 * DeadeyeJsonAdapter}.
 * <p>
 * Classes {@code Point} and {@code Point2D} are utility classes used by the {@code TargetData}
 * classes.
 */
package org.strykeforce.deadeye;