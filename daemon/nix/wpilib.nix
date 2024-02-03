{ lib
, stdenv
, fetchFromGitHub
, cmake
, opencv
}:
let
  opencv-gstreamer = opencv.overrideAttrs {
    enableGStreamer = true;
    enableContrib = false;
  };

  wpilibFlag = name: enabled: "-DWITH_${name}=${if enabled then "ON" else "OFF"}";
in
stdenv.mkDerivation rec {
  pname = "wpilib";
  version = "2022.4.1";

  src = fetchFromGitHub {
    owner = "wpilibsuite";
    repo = "allwpilib";
    rev = "v${version}";
    sha256 = "sha256-8BMUTAGJwUPp3butmn+jMHNUS0Swtv9BqNBnAxJQ9KA";
  };

  outputs = [ "out" ];

  buildInputs = [ stdenv.cc.cc.lib ];

  nativeBuildInputs = [ cmake ];

  propagatedBuildInputs = [ opencv-gstreamer ];

  cmakeFlags = [
    (wpilibFlag "JAVA" false)
    (wpilibFlag "CSCORE" true)
    (wpilibFlag "WPIMATH" false)
    (wpilibFlag "WPILIB" false)
    (wpilibFlag "OLD_COMMANDS" false)
    (wpilibFlag "EXAMPLES" false)
    (wpilibFlag "TESTS" false)
    (wpilibFlag "GUI" false)
    (wpilibFlag "SIMULATION_MODULES" false)
  ];

  NIX_CFLAGS_COMPILE = [
    "-Wno-error=maybe-uninitialized"
    "-Wno-error=dangling-pointer="
    "-Wno-error=dangling-reference"
  ];

}


