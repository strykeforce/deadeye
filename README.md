# Deadeye

Stryke Force Deadeye Vision App

```
/Deadeye
  /Control
    /Camera0
      /Active - bool
  /Config
    /Camera0
      /HSV - double[3]
```

https://stackoverflow.com/questions/46219454/how-to-open-a-gstreamer-pipeline-from-opencv-with-videowriter

```cpp
cv::VideoCapture cap("v4l2src device=/dev/video1 ! videoscale ! videorate ! video/x-raw, width=640, height=360, framerate=30/1 ! videoconvert ! appsink");
cv::imshow("feed", frame);
```
