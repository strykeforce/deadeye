{
  description = "Deadeye daemon dev environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";

    safe = {
      url = "github:LouisCharlesC/safe";
      flake = false;
    };

    tinyfsm = {
      url = "github:jhh/tinyfsm/cmake";
      flake = false;
    };

    readerwriterqueue = {
      url = "github:cameron314/readerwriterqueue";
      flake = false;
    };

    wpilib-src = {
      url = "github:wpilibsuite/allwpilib?ref=v2022.4.1";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, flake-utils, safe, tinyfsm, readerwriterqueue, wpilib-src }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
          buildHeaderOnlyLib = name: version: src: pkgs.gcc7Stdenv.mkDerivation
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

          wpilib = pkgs.gcc7Stdenv.mkDerivation {
            pname = "wpilib";
            version = "2022.4.1";
            src = wpilib-src;
            nativeBuildInputs = [
              pkgs.cmake
            ];
            buildInputs = [
              pkgs.gcc-unwrapped
            ];
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

            hardeningDisable = [ "all" ];

            outputs = [ "out" ];

            doCheck = true;
            preCheck = "export LD_LIBRARY_PATH=$(pwd):$LD_LIBRARY_PATH";
          };
        in
        {
          devShell = pkgs.gcc7Stdenv.mkDerivation {
            name = "shell";
            buildInputs = [
              pkgs.cmake
              pkgs.ninja
              pkgs.gcc-unwrapped
              pkgs.cmakeCurses

              pkgs.catch2
              pkgs.gst_all_1.gst-plugins-base
              pkgs.gst_all_1.gst-plugins-good
              pkgs.nlohmann_json
              pkgs.pkg-config
              pkgs.spdlog.dev
              wpilib
              (buildHeaderOnlyLib "readerwriterqueue" "1.0.5" readerwriterqueue)
              (buildHeaderOnlyLib "safe" "1.0.0" safe)
              (buildHeaderOnlyLib "tinyfsm" "0.3.3" tinyfsm)
            ] ++ pkgs.lib.optional pkgs.stdenv.isLinux [
              pkgs.systemd.dev
            ];
          };

          packages.default =
            let
              gst = with pkgs.gst_all_1; [ gstreamer gst-plugins-base gst-plugins-good ];
              gstPluginPath = "--prefix GST_PLUGIN_PATH : ${pkgs.lib.makeSearchPathOutput "lib" "lib/gstreamer-1.0" gst}";
            in
            pkgs.stdenv.mkDerivation rec {
              pname = "deadeye-daemon";
              version = "22.2.0";
              src = ./.;

              nativeBuildInputs = [
                pkgs.cmake
                pkgs.pkg-config
                pkgs.makeWrapper
              ];

              buildInputs = [
                pkgs.catch2
                pkgs.nlohmann_json
                pkgs.spdlog.dev
                wpilib
                (buildHeaderOnlyLib "readerwriterqueue" "1.0.5" readerwriterqueue)
                (buildHeaderOnlyLib "safe" "1.0.0" safe)
                (buildHeaderOnlyLib "tinyfsm" "0.3.3" tinyfsm)
              ] ++ pkgs.lib.optional pkgs.stdenv.isLinux pkgs.systemd.dev;

              cmakeFlags = [
                "-DCMAKE_BUILD_TYPE=Release"
                "-DDEADEYE_BUILD_TESTS=OFF"
                "-DDEADEYE_BUILD_BENCHMARKS=OFF"
                "-DDEADEYE_UNIT_ID=N"
                "-DDEADEYE_NT_SERVER=127.0.0.1"
                "-DDEADEYE_CAMERA0_PIPELINE=deadeye::NullPipeline"
              ];

              postFixup = ''
                wrapProgram "$out/bin/deadeyed" ${gstPluginPath}
              '';
            };
        }
      ) // {
      nixosModules.default = { config, lib, pkgs, ... }:
        with lib;
        let
          cfg = config.deadeye.daemon;
        in
        {
          options.deadeye.daemon = {
            enable = mkEnableOption "Enable the Deadeye pipeline daemon service";

            unitId = mkOption {
              description = "Deadeye Unit ID";
              type = types.str;
              default = "Z";
              example = "A";
            };

            pipeline0 = mkOption {
              description = "Deadeye Pipeline 0";
              type = types.str;
              default = "deadeye::NullPipeline";
              example = "deadeye::UprightRectPipeline";
            };

            pipeline1 = mkOption {
              description = "Deadeye Pipeline 0";
              type = types.str;
              default = "";
              example = "deadeye::UprightRectPipeline";
            };

            pipeline2 = mkOption {
              description = "Deadeye Pipeline 0";
              type = types.str;
              default = "";
              example = "deadeye::UprightRectPipeline";
            };

            pipeline3 = mkOption {
              description = "Deadeye Pipeline 0";
              type = types.str;
              default = "";
              example = "deadeye::UprightRectPipeline";
            };

            pipeline4 = mkOption {
              description = "Deadeye Pipeline 0";
              type = types.str;
              default = "";
              example = "deadeye::UprightRectPipeline";
            };

            streamAddress = mkOption {
              description = "Address of the camera stream.";
              type = types.str;
              default = "0.0.0.0";
            };

            ntServerAddress = mkOption {
              description = "Address of the NetworkTables server";
              type = types.str;
              default = "127.0.0.1";
            };

            ntServerPort = mkOption {
              description = "Port the NetworkTables server listens on.";
              type = types.port;
              default = 1735;
            };

            logLevel = mkOption {
              description = "Logging level";
              type = types.enum [ "trace" "debug" "info" "warn" "error" "critical" ];
              default = "info";
              example = "debug";
            };
          };

          config = mkIf cfg.enable {
            systemd.services.deadeye-daemon = {
              wantedBy = [ "multi-user.target" ];

              environment = {
                DEADEYE_NT_PORT = "${toString cfg.ntServerPort}";
                DEADEYE_NT_SERVER = cfg.ntServerAddress;
                SPDLOG_LEVEL = cfg.logLevel;
              };

              serviceConfig =
                let pkg = self.packages.${pkgs.system}.default.overrideAttrs (oldAttrs: rec {
                  cmakeFlags = [
                    "-DCMAKE_BUILD_TYPE=Release"
                    "-DDEADEYE_BUILD_TESTS=OFF"
                    "-DDEADEYE_BUILD_BENCHMARKS=OFF"
                    "-DDEADEYE_UNIT_ID=${cfg.unitId}"
                    "-DDEADEYE_NT_SERVER=${cfg.ntServerAddress}"
                    "-DDEADEYE_CAMERA0_PIPELINE=${cfg.pipeline0}"
                    "-DDEADEYE_CAMERA1_PIPELINE=${cfg.pipeline1}"
                    "-DDEADEYE_CAMERA2_PIPELINE=${cfg.pipeline2}"
                    "-DDEADEYE_CAMERA3_PIPELINE=${cfg.pipeline3}"
                    "-DDEADEYE_CAMERA4_PIPELINE=${cfg.pipeline4}"
                    "-DDEADEYE_STREAM_ADDRESS=${cfg.streamAddress}"
                  ];
                });
                in
                {
                  Restart = "on-failure";
                  ExecStart = "${pkg}/bin/deadeyed";
                };
            };
          };
        };
      #
      nixosConfigurations.container = nixpkgs.lib.nixosSystem
        {
          system = "x86_64-linux";
          modules = [
            self.nixosModules.default
            ({ config, pkgs, lib, ... }: {
              # Only allow this to boot as a container
              boot.isContainer = true;
              networking.hostName = "deadeye";

              deadeye.daemon.enable = true;
              deadeye.daemon.unitId = "A";
              deadeye.daemon.pipeline0 = "deadeye::MinAreaRectPipeline";
              deadeye.daemon.pipeline1 = "deadeye::UprightRectPipeline";
              deadeye.daemon.ntServerAddress = "192.168.1.30";
              deadeye.daemon.logLevel = "trace";
              deadeye.daemon.streamAddress = "0.0.0.0";
            })
          ];
        };
    };
}
