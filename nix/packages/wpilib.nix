{
  pkgs,
  perSystem,
}:
let
  opencv-gstreamer = pkgs.opencv.overrideAttrs {
    enableGStreamer = true;
    enableContrib = false;
  };

  wpilibFlag = name: enabled: "-DWITH_${name}=${if enabled then "ON" else "OFF"}";
in
pkgs.stdenv.mkDerivation rec {
  pname = "wpilib";
  version = "2022.4.1";

  src = pkgs.fetchFromGitHub {
    owner = "wpilibsuite";
    repo = "allwpilib";
    rev = "v${version}";
    sha256 = "sha256-8BMUTAGJwUPp3butmn+jMHNUS0Swtv9BqNBnAxJQ9KA";
  };

  outputs = [ "out" ];

  buildInputs = [
    perSystem.self.fmt-8.dev
    pkgs.stdenv.cc.cc.lib
  ];

  nativeBuildInputs = [ pkgs.cmake ];

  propagatedBuildInputs = [ opencv-gstreamer ];

  cmakeFlags = [
    # TODO: check if needed when upgrading
    "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
    # use fmt from buildInputs
    "-DUSE_VCPKG_FMTLIB=ON"
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
    "-Wno-error=deprecated-declarations"
    "-Wno-error=unused-but-set-variable"
    "-Wno-error=deprecated-literal-operator"
    "-Wno-error=unnecessary-virtual-specifier"
  ];

}
