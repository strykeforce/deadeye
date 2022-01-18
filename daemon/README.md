# Deadeye

Stryke Force Deadeye Vision App

## Build Dependencies

### macOS

We'll use Homebrew to satisfy dependencies.

#### OpenCV

Run `brew edit opencv` and make the following edits:

```diff
diff --git a/Formula/opencv.rb b/Formula/opencv.rb
index 3f5d7da1ed7..1b885751e1f 100644
--- a/Formula/opencv.rb
+++ b/Formula/opencv.rb
@@ -38,6 +38,13 @@ class Opencv < Formula
   depends_on "tbb"
   depends_on "vtk"
   depends_on "webp"
+  # jhh 2022-01-16
+  depends_on "gstreamer"
+  depends_on "gst-plugins-base"
+  depends_on "gst-plugins-good"
+  depends_on "gst-plugins-bad"
+  depends_on "gst-plugins-ugly"
+  depends_on "gst-libav"

   uses_from_macos "zlib"

@@ -81,7 +88,7 @@ class Opencv < Formula
       -DWITH_EIGEN=ON
       -DWITH_FFMPEG=ON
       -DWITH_GPHOTO2=OFF
-      -DWITH_GSTREAMER=OFF
+      -DWITH_GSTREAMER=ON
       -DWITH_JASPER=OFF
       -DWITH_OPENEXR=ON
       -DWITH_OPENGL=OFF
```

Then build and install:

```sh
$ brew install --build-from-source opencv
```

Verify that GStreamer was compiled in:

```
❯ /usr/local/bin/python3
Python 3.9.10 (main, Jan 15 2022, 11:48:04)
[Clang 13.0.0 (clang-1300.0.29.3)] on darwin
Type "help", "copyright", "credits" or "license" for more information.
>>> import cv2
>>> print(cv2.getBuildInformation())
```

#### Other dependencies


Install the following homebrew dependencies after installing OpenCV:

```
$ brew install spdlog nlohmann-json catch2
$ # the following will build from source
$ brew install strykeforce/deadeye/louischarlesc-safe strykeforce/deadeye/readerwriterqueue
$ brew install strykeforce/deadeye/tinyfsm strykeforce/deadeye/wpilib
```

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

## Jetson Nano Prep

1. Download and install the system image by following the [
   Getting Started With Jetson Nano Developer Kit](https://developer.nvidia.com/embedded/learn/get-started-jetson-nano-devkit#intro) guide.
2. Temporarily attach a HDMI monitor, keyboard and mouse to accept license and complete first boot setup.
    - By convention, we name each board with its Deadeye unit name, for example `deadeye-a`, `deadeye-b`, etc.
    - Create a user account for the team member that is maintaining the Deadeye system.
    - Install above user's public SSH key in `~/.ssh/authorized_keys`.
3. Reboot and verify system is reachable over SSH.
4. Turn off graphical boot with `sudo systemctl set-default multi-user.target`
5. Update the system with `sudo apt update`, `sudo apt upgrade`, sudo apt autoremove`.
6. Allow `sudo` without specifying password (for Ansible) with `sudo visudo` and changing line to: `%sudo ALL=(ALL:ALL) NOPASSWD: ALL`.
7. Ensure `pipenv` installed and provision from `ansible/` directory with `pipenv run provision -l <deadeye-unit>`, where `<deadeye-unit>` is the host entry in `inventory.yml` for the Jetson you are prepping.
8. Deploy with `pipenv run deploy`, optionally limiting hosts as above.
