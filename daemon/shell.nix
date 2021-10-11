{ sources ? import nix/sources.nix }:
let
  pkgs = import sources.nixpkgs { };
  libsafe = pkgs.callPackage ./nix/safe.nix { inherit sources; };
  readerwriterqueue = pkgs.callPackage ./nix/readerwriterqueue.nix { inherit sources; };
  tinyfsm = pkgs.callPackage ./nix/tinyfsm.nix { inherit sources; };
  wpilib = pkgs.callPackage ./nix/wpilib.nix { inherit sources; };
in
pkgs.mkShell {
  buildInputs = [
    pkgs.catch2
    pkgs.cmakeCurses
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
