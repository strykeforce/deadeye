**********
Quickstart
**********

This section gets you started using Deadeye with a FRC robot program. It will
work on either a robot or the `robot simulator
<https://docs.wpilib.org/en/latest/docs/software/wpilib-tools/robot-simulation/index.html>`_
available on your development workstation.


.. figure:: images/target.jpg
   :align: center
   :width: 60%

   test target image

   In this example, we'll upload this target `test image <_images/target.jpg>`_ to Deadeye.


.. spacer

Pipeline
========

This example assumes you have a Deadeye unit configured with the
``UprightRectPipeline`` and you can access its admin dashboard from your
browser.

Capture
-------

Start by using :menuselection:`Capture --> Type --> Image Upload` dashboard
option to upload a `test image <_images/target.jpg>`_ of a target.

.. image:: images/dash-img-upload.png
   :width: 100%
   :align: center

Mask
----

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
bounds until target pixels just start to disappear.

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

Installation
============

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
