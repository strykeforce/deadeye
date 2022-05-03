{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
    n2n.url = "github:jhh/node2nix";

    safe-src = {
      url = "github:LouisCharlesC/safe";
      flake = false;
    };

    tinyfsm-src = {
      url = "github:jhh/tinyfsm/cmake";
      flake = false;
    };

    readerwriterqueue-src = {
      url = "github:cameron314/readerwriterqueue";
      flake = false;
    };

    wpilib-src = {
      url = "github:wpilibsuite/allwpilib?ref=v2022.4.1";
      flake = false;
    };

  };

  outputs = { self, nixpkgs, utils, n2n, safe-src, tinyfsm-src, readerwriterqueue-src, wpilib-src }:
    let
      version = "22.2.0";

      out = system:
        let
          pkgs = import nixpkgs
            {
              inherit system;
              overlays = [ n2n.overlays.${system} ];
            };

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
        in
        {

          devShells.default = pkgs.mkShell {
            nativeBuildInputs = with pkgs; [
              # daemon
              cmake
              cmakeCurses
              ninja

              # admin
              poetry

              # web
              nodejs-16_x
              node2nix
              nodePackages.http-server
            ];

            buildInputs = [
              pkgs.catch2
              pkgs.nlohmann_json
              pkgs.pkg-config
              pkgs.spdlog.dev
              self.packages.${system}.wpilib
            ];
          };

          packages.admin = with pkgs.poetry2nix; mkPoetryApplication {
            projectDir = ./admin;
            preferWheels = true;
          };

          packages.web =
            let
              nodeDependencies = (pkgs.callPackage ./web/config/default.nix { }).nodeDependencies;
            in
            pkgs.stdenv.mkDerivation {
              pname = "deadeye-web";
              inherit version;
              src = ./web;

              nativeBuildInputs = with pkgs; [ nodejs-16_x ];

              phases = "buildPhase";

              buildPhase = ''
                export PATH="${nodeDependencies}/bin:$PATH"
                export NODE_PATH="${nodeDependencies}/lib/node_modules"
                export NODE_ENV=production
                export BUILD_DIR="$out"
                ln -s $src/src .
                ln -s $src/config .
                ln -s $src/package.json .
                ln -s $src/webpack.config.js .
                ln -s $src/postcss.config.json .
                ln -s $NODE_PATH .

                npm run build
              '';
            };

          packages.wpilib = pkgs.stdenv.mkDerivation {
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

            outputs = [ "out" ];

            doCheck = true;
            preCheck = "export LD_LIBRARY_PATH=$(pwd):$LD_LIBRARY_PATH";
          };


          packages.daemon =
            let
              gst = with pkgs.gst_all_1; [ gstreamer gst-plugins-base gst-plugins-good ];
              gstPluginPath = "--prefix GST_PLUGIN_PATH : ${pkgs.lib.makeSearchPathOutput "lib" "lib/gstreamer-1.0" gst}";
            in
            pkgs.stdenv.mkDerivation rec {
              pname = "deadeye-daemon";
              inherit version;
              src = ./daemon;

              nativeBuildInputs = [
                pkgs.cmake
                pkgs.pkg-config
                pkgs.makeWrapper
              ];

              buildInputs = [
                pkgs.catch2
                pkgs.nlohmann_json
                pkgs.spdlog.dev
                self.packages.${system}.wpilib
                (buildHeaderOnlyLib "readerwriterqueue" "1.0.5" readerwriterqueue-src)
                (buildHeaderOnlyLib "safe" "1.0.0" safe-src)
                (buildHeaderOnlyLib "tinyfsm" "0.3.3" tinyfsm-src)
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




        };
    in
    with utils.lib; eachSystem defaultSystems out // {

      nixosModules.default = { config, lib, pkgs, ... }:
        with lib;
        let
          cfg = config.deadeye;
        in
        {
          options.deadeye = {
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
          };

          options.deadeye.admin = {
            enable = mkEnableOption "Enable the Deadeye admin middleware service";

            port = mkOption {
              description = "Port the server listens on.";
              type = types.port;
              default = 5000;
            };

            uploadDir = mkOption {
              description = "Directory used for image uploads.";
              type = types.str;
              default = "/tmp/deadeye";
            };
          };

          options.deadeye.web = {
            enable = mkEnableOption "Enable the Deadeye web console";

            adminAddress = mkOption {
              description = "Address of the Deadeye admin middleware service";
              type = types.str;
              default = "127.0.0.1";
            };

            domain = mkOption rec {
              type = types.str;
              default = "deadeye.strykeforce.org";
              example = default;
              description = "The domain name for Deadeye web console";
            };
          };

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

            logLevel = mkOption {
              description = "Logging level";
              type = types.enum [ "trace" "debug" "info" "warn" "error" "critical" ];
              default = "info";
              example = "debug";
            };
          };



          config = mkMerge [
            (mkIf cfg.admin.enable {
              systemd.tmpfiles.rules = [ "d ${cfg.admin.uploadDir} 1777 root root 1d" ];

              systemd.services.deadeye-admin = {
                wantedBy = [ "multi-user.target" ];

                environment = {
                  DEADEYE_NT_SERVER = cfg.ntServerAddress;
                  DEADEYE_NT_PORT = "${toString cfg.ntServerPort}";
                  DEADEYE_ADMIN_PORT = "${toString cfg.admin.port}";
                  DEADEYE_NT_WAIT_MS = "500";
                  DEADEYE_UPLOAD_DIR = cfg.admin.uploadDir;
                  FLASK_ENV = "production";
                };

                serviceConfig =
                  let pkg = self.packages.${pkgs.system}.admin;
                  in
                  {
                    Restart = "on-failure";
                    ExecStart = "${pkg}/bin/deadeye-server";
                  };
              };
            })

            (mkIf cfg.web.enable {
              services.nginx.enable = true;
              services.nginx.recommendedProxySettings = true;
              services.nginx.virtualHosts.${cfg.web.domain} = {
                default = true;

                locations = {
                  "/" = {
                    root = "${self.packages.${pkgs.system}.web}";
                    index = "index.html index.htm";
                    tryFiles = "$uri $uri/ /index.html";
                  };

                  "/socket.io/" = {
                    proxyPass = "http://${cfg.web.adminAddress}:${toString cfg.admin.port}";
                    proxyWebsockets = true;
                  };

                  "/upload" = {
                    proxyPass = "http://${cfg.web.adminAddress}:${toString cfg.admin.port}";
                  };

                  # "/stream/0/" = { proxyPass = "http://127.0.0.1:5805/stream.mjpg"; };
                  # "/stream/1/" = { proxyPass = "http://127.0.0.1:5806/stream.mjpg"; };
                  # "/stream/2/" = { proxyPass = "http://127.0.0.1:5807/stream.mjpg"; };
                  # "/stream/3/" = { proxyPass = "http://127.0.0.1:5808/stream.mjpg"; };
                  # "/stream/4/" = { proxyPass = "http://127.0.0.1:5809/stream.mjpg"; };
                };
              };
            })

            (mkIf cfg.daemon.enable {
              systemd.services.deadeye-daemon = {
                wantedBy = [ "multi-user.target" ];

                environment = {
                  DEADEYE_NT_PORT = "${toString cfg.ntServerPort}";
                  DEADEYE_NT_SERVER = cfg.ntServerAddress;
                  SPDLOG_LEVEL = cfg.daemon.logLevel;
                };

                serviceConfig =
                  let pkg = self.packages.${pkgs.system}.daemon.overrideAttrs (oldAttrs: rec {
                    cmakeFlags = [
                      "-DCMAKE_BUILD_TYPE=Release"
                      "-DDEADEYE_BUILD_TESTS=OFF"
                      "-DDEADEYE_BUILD_BENCHMARKS=OFF"
                      "-DDEADEYE_UNIT_ID=${cfg.daemon.unitId}"
                      "-DDEADEYE_NT_SERVER=${cfg.ntServerAddress}"
                      "-DDEADEYE_CAMERA0_PIPELINE=${cfg.daemon.pipeline0}"
                      "-DDEADEYE_CAMERA1_PIPELINE=${cfg.daemon.pipeline1}"
                      "-DDEADEYE_CAMERA2_PIPELINE=${cfg.daemon.pipeline2}"
                      "-DDEADEYE_CAMERA3_PIPELINE=${cfg.daemon.pipeline3}"
                      "-DDEADEYE_CAMERA4_PIPELINE=${cfg.daemon.pipeline4}"
                      "-DDEADEYE_STREAM_ADDRESS=${cfg.daemon.streamAddress}"
                    ];
                  });
                  in
                  {
                    Restart = "on-failure";
                    ExecStart = "${pkg}/bin/deadeyed";
                  };
              };
            })
          ];

        };



      nixosConfigurations.container = nixpkgs.lib.nixosSystem {
        system = "x86_64-linux";
        modules = [
          self.nixosModules.default
          ({ config, pkgs, ... }: {
            # Only allow this to boot as a container
            boot.isContainer = true;
            networking.hostName = "deadeye-admin";

            # Allow nginx through the firewall
            networking.firewall.allowedTCPPorts = [ config.deadeye.admin.port ];

            deadeye.admin.enable = false;
            deadeye.web.enable = true;
            deadeye.ntServerAddress = "192.168.1.30";
            deadeye.daemon.enable = true;
            deadeye.daemon.unitId = "A";
            deadeye.daemon.pipeline0 = "deadeye::MinAreaRectPipeline";
            deadeye.daemon.pipeline1 = "deadeye::UprightRectPipeline";
            deadeye.daemon.logLevel = "trace";
            deadeye.daemon.streamAddress = "0.0.0.0";
          })
        ];
      };
    };
}


