***********
Basic Usage
***********

Java Client Library
===================

Pipelines are designed to be used with their corresponding ``TargetData`` type.

A simple example of connecting to a Deadeye camera, A0, configured with ``deadeye::UprightRectPipeline`` and ``UprightRectTargetData``:

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