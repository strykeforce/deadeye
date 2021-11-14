{
  description = "Deadeye develop environment";

  inputs = {
    nixpkgs.url = github:NixOS/nixpkgs;
    flake-utils.url = github:numtide/flake-utils;

    safe = {
      url = github:LouisCharlesC/safe;
      flake = false;
    };

    tinyfsm = {
      url = github:jhh/tinyfsm/cmake;
      flake = false;
    };

    readerwriterqueue = {
      url = github:cameron314/readerwriterqueue;
      flake = false;
    };

    wpilib-src = {
      url = github:wpilibsuite/allwpilib?ref=v2021.3.1;
      flake = false;
    };
  };

  outputs = { self, nixpkgs, flake-utils, safe, tinyfsm, readerwriterqueue, wpilib-src }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        buildHeaderOnlyLib = name: version: src: pkgs.stdenv.mkDerivation
          rec {
            pname = name;
            inherit src version;
            nativeBuildInputs = [ pkgs.cmake ];
            cmakeFlags = [
              "-DCMAKE_BUILD_TYPE=Release"
            ];
            outputs = [ "out" ];
            doCheck = false;
          };

        wpilib = pkgs.stdenv.mkDerivation {
          pname = "wpilib";
          version = "2021.3.1";
          src = wpilib-src;
          nativeBuildInputs = [ pkgs.cmake ];
          buildInputs = [ pkgs.gcc-unwrapped ];
          propagatedBuildInputs = [ pkgs.opencv4 ];

          cmakeFlags = [
            "-DCMAKE_BUILD_TYPE=Release"
            "-DWITH_JAVA=OFF"
            "-DWITH_CSCORE=ON"
            "-DWITH_WPIMATH=OFF"
            "-DWITH_WPILIB=OFF"
            "-DWITH_OLD_COMMANDS=OFF"
            "-DWITH_EXAMPLES=OFF"
            "-DWITH_TESTS=OFF"
            "-DWITH_GUI=OFF"
            "-DWITH_SIMULATION_MODULES=OFF"
          ];

          outputs = [ "out" ];

          doCheck = true;
          preCheck = "export LD_LIBRARY_PATH=$(pwd):$LD_LIBRARY_PATH";
        };
      in
      {
        devShell = pkgs.mkShell {
          buildInputs = [
            pkgs.catch2
            pkgs.cmakeCurses
            pkgs.gst_all_1.gst-plugins-base
            pkgs.gst_all_1.gstreamer
            pkgs.jdk11
            pkgs.mdbook
            pkgs.ninja
            pkgs.nlohmann_json
            pkgs.python39Packages.poetry
            pkgs.spdlog
            (buildHeaderOnlyLib "readerwriterqueue" "1.0.5" readerwriterqueue)
            (buildHeaderOnlyLib "safe" "1.0.0" safe)
            (buildHeaderOnlyLib "tinyfsm" "0.3.3" tinyfsm)
            wpilib
          ] ++ (if pkgs.stdenv.isLinux then [ pkgs.pkg-config pkgs.systemd.dev ] else [ ]);
        };
      }
    );
}
