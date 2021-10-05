**********
Quickstart
**********

This section gets you started using Deadeye with a FRC robot program. It will
work on either a robot or the `robot simulator
<https://docs.wpilib.org/en/latest/docs/software/wpilib-tools/robot-simulation/index.html>`_
available on your development workstation.

In this example, we'll upload this target `test image <_images/target.jpg>`_ to
Deadeye. To make things interesting, this image includes extra retro-reflective
tape reflections ("noise") that will need to be filtered out to isolate the target
correctly.

.. figure:: images/target.jpg
   :align: center
   :width: 60%

   test target image

Configure Pipeline
==================

This example assumes you have a Deadeye unit configured with the
``UprightRectPipeline`` and you can access its admin dashboard from your
browser.

.. note:: The quickstart pipeline tuning process outline below is good enough
   for our single target test image. Real-world usage requires more careful
   tuning that takes into account varying field positions and lighting
   conditions.

Image Capture
-------------

Start by using :menuselection:`Capture --> Type --> Image Upload` dashboard
option to upload a `test image <_images/target.jpg>`_ of a target.

.. image:: images/dash-img-upload.png
   :width: 100%
   :align: center

Mask Target
-----------

.. TODO: make these glossary terms

Aquire *contours* that surround target reflections by adjusting the pipeline
*mask* parameters. 

.. note:: You can make the *hue*, *saturation*, and *value* adjustments in any
   order, but we usually adjust value last and keep as wide a value range as
   possible to account for different target viewing distances.

Since the reflected light from the retro-reflective tape is a saturated color
we start by adjusting the :menuselection:`Mask --> Saturation` lower bound.
While watching the preview, increase the lower bound to a value that starts to
remove low-saturation pixels but retains all the target pixels that we are
interested in.

.. figure:: images/dash-mask-sat.png
   :width: 100%
   :align: center

   Mask Saturation lower bound set to 100.

We use a specific green-colored light to illuminate the target so we can filter
out other colors by adjusting the :menuselection:`Mask --> Hue` lower and upper
bounds until just before the target pixels start to disappear.

.. figure:: images/dash-mask-hue.png
   :width: 100%
   :align: center

   Mask Hue bounds set to 65—100.


Finally, since the retro-reflective target tape creates a bright reflection, we
adjust the :menuselection:`Mask --> Value` lower-bound to clean up the
reflected target mask.

.. figure:: images/dash-mask-val.png
   :width: 100%
   :align: center

   Mask Value lower bound set to 65.

Filter Contours
---------------

Our masked test target image contains spurious contours that need to be
filtered out. Set :menuselection:`Preview --> Contours --> Filtered` and
adjust the filters until you just have a single contour around the target left.

.. note:: When tuning filters for an actual robot, some of the filter
   parameters may be affected by camera viewing angle and distance as the robot
   moves around the field. Be sure to test thoroughly!

.. figure:: images/dash-filter.png
   :width: 100%
   :align: center

   Contour aspect ratio and solidity filters adjusted.

Pipeline Logging
----------------

Logging can be enabled in the dashboard to assist troubleshooting during
competitions. Here's an example of logging output from the pipeline we just
configured.

.. figure:: images/1-1.jpg
   :width: 100%
   :align: center

   Deadeye pipeline *four-up* logging output.

Configure Robot Project
=======================

Deadeye provides a Java client library that can be used to control and receive
data from a Deadeye unit.

Subsection
----------

For FRC robot projects, use the Deadeye client library `deadeye.json
<http://maven.strykeforce.org/deadeye.json>`_ vendordeps file.

To install, use **Install new libraries (online)** in VS Code or download
manually to your project vendordeps directory.

Alternatively, you can run the following Gradle task in your project directory
to download and install the Deadeye vendor dependency file:

.. code-block:: console

    $ ./gradlew vendordep --url=http://maven.strykeforce.org/deadeye.json

Usage
=====

Pipelines are designed to be used with their corresponding ``TargetData`` type.

A simple example of connecting to a Deadeye camera, A0, configured with
``deadeye::UprightRectPipeline`` and ``UprightRectTargetData``:

.. code-block:: Java

    public class DeadeyeA0 implements TargetDataListener<UprightRectTargetData> {

        private Deadeye<UprightRectTargetData> deadeye = new Deadeye<>("A0", UprightRectTargetData.class);

        public DeadeyeA0() {
            deadeye.setTargetDataListener(this);
        }

        // start/stop pipeline
        public void setEnabled(boolean enabled) {
            deadeye.setEnabled(enabled);
        }

        // called for every video frame
        @Override
        public void onTargetData(UprightRectTargetData data) {
            Point center = data.center;
            System.out.printf("x=%d, y=%d %n", center.x, center.y);
        }

    }
