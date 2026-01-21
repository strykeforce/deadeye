# Deadeye

Stryke Force Deadeye Vision App

## Build Dependencies

All dependencies are provided in the Nix environment.

## Network Tables

```
/Deadeye
  /ControlA
    /Camera0
      /On - bool
      /Off - bool
      /Error - bool
      /Lights
        /On - bool
        /Off - bool
        /Blink - bool
  /ConfigA
    /Camera0
      /HSV - double[3]
```

<https://stackoverflow.com/questions/46219454/how-to-open-a-gstreamer-pipeline-from-opencv-with-videowriter>

```cpp
cv::VideoCapture cap("v4l2src device=/dev/video1 ! videoscale ! videorate ! video/x-raw, width=640, height=360, framerate=30/1 ! videoconvert ! appsink");
cv::imshow("feed", frame);
```

```
gst-launch-1.0 videotestsrc num-buffers=1 ! \
  'video/x-raw, width=(int)640, height=(int)480, format=(string)I420' ! jpegenc ! filesink location=test.jpg -e
```
