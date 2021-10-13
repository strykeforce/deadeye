{ sources ? import ../nix/sources.nix }:
let
  pkgs = import sources.nixpkgs { };
  libsafe = pkgs.callPackage ../nix/safe.nix { };
  readerwriterqueue = pkgs.callPackage ../nix/readerwriterqueue.nix { };
  tinyfsm = pkgs.callPackage ../nix/tinyfsm.nix { };
  wpilib = pkgs.callPackage ../nix/wpilib.nix { };
in
pkgs.mkShell {
  buildInputs = [
    pkgs.catch2
    pkgs.cmakeCurses
    pkgs.gst_all_1.gstreamer
    pkgs.gst_all_1.gst-plugins-base
    libsafe
    pkgs.ninja
    pkgs.nlohmann_json
    pkgs.pkg-config
    readerwriterqueue
    pkgs.spdlog
    pkgs.systemd.dev
    tinyfsm
    wpilib
  ];
}
