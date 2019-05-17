# Deadeye

Stryke Force Deadeye Vision App

# Build Dependencies

The following should be installed via CMake:

- [ntcore & cscore](https://github.com/wpilibsuite/allwpilib)
- [spdlog](https://github.com/gabime/spdlog)
- [Catch2](https://github.com/catchorg/Catch2)
- [nlohmann/json](https://github.com/nlohmann/json)

These may be installed or need to be installed:

- OpenCV: see below

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

https://stackoverflow.com/questions/46219454/how-to-open-a-gstreamer-pipeline-from-opencv-with-videowriter

```cpp
cv::VideoCapture cap("v4l2src device=/dev/video1 ! videoscale ! videorate ! video/x-raw, width=640, height=360, framerate=30/1 ! videoconvert ! appsink");
cv::imshow("feed", frame);
```

```
gst-launch-1.0 videotestsrc num-buffers=1 ! \
  'video/x-raw, width=(int)640, height=(int)480, format=(string)I420' ! jpegenc ! filesink location=test.jpg -e
```

## OpenCV

Build OpenCV on macOS to enable Gstreamer. Need to environment variables per
`brew info libffi`. Right now OpenCV dependency is pulled in via cscore, so
rebuild cscore when building. Tests failed to compile, fixed by disabling.
