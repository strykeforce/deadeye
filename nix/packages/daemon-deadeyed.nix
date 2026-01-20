{
  inputs,
  perSystem,
  pkgs,
}:
let
  gst = with pkgs.gst_all_1; [
    gstreamer
    gst-plugins-base
    gst-plugins-good
  ];
  gstPlugins = pkgs.lib.makeSearchPathOutput "lib" "lib/gstreamer-1.0" gst;
  filter = inputs.nix-filter.lib;
  libPathEnv = if pkgs.stdenv.isDarwin then "DYLD_LIBRARY_PATH" else "LD_LIBRARY_PATH";
in
pkgs.stdenv.mkDerivation {
  pname = "deadeye-daemon";
  version = "22.2.0";
  src = filter {
    root = ../../daemon;
    include = [
      "src"
      "cmake"
      "CMakeLists.txt"
      "version.cmake"
    ];
  };

  nativeBuildInputs = with pkgs; [
    cmake
    pkg-config
    makeWrapper
  ];

  buildInputs =
    with pkgs;
    [
      catch2
      nlohmann_json
      perSystem.self.spdlog.dev
      perSystem.self.readerwriterqueue
      perSystem.self.safe
      perSystem.self.tinyfsm
      perSystem.self.wpilib
    ]
    ++ lib.optionals stdenv.isLinux [
      libgpiod_1
      systemd
    ];

  cmakeFlags = [
    "-DDEADEYE_BUILD_TESTS=OFF"
    "-DDEADEYE_BUILD_BENCHMARKS=OFF"
    "-DDEADEYE_UNIT_ID=W"
    "-DDEADEYE_NT_SERVER=192.168.1.165"
    "-DDEADEYE_STREAM_ADDRESS=192.168.1.165"
    "-DDEADEYE_CAMERA0_PIPELINE=deadeye::TargetListPipeline"
    "-DDEADEYE_CAMERA1_PIPELINE=deadeye::UprightRectPipeline"
  ];

  postFixup = ''
    wrapProgram "$out/bin/deadeyed" \
      --prefix GST_PLUGIN_PATH : ${gstPlugins} \
      --set ${libPathEnv} ${perSystem.self.wpilib}/wpilib/lib/
  '';

}
