{ sources ? import nix/sources.nix }:
let
  pkgs = import sources.nixpkgs { };
  wpilib = pkgs.callPackage ./nix/wpilib.nix { inherit sources; };
  libsafe = pkgs.callPackage ./nix/safe.nix { inherit sources; };
  tinyfsm = pkgs.callPackage ./nix/tinyfsm.nix { inherit sources; };
  readerwriterqueue = pkgs.callPackage ./nix/readerwriterqueue.nix { inherit sources; };
in
pkgs.mkShell {
  buildInputs = with pkgs; [
    catch2
    cmakeCurses
    libsafe
    ninja
    nlohmann_json
    pkg-config
    readerwriterqueue
    spdlog
    systemd.dev
    tinyfsm
    wpilib
  ];
}
