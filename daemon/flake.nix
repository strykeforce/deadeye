{
  description = "Deadeye vision pipeline daemon";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    nix-filter.url = "github:numtide/nix-filter";
  };

  outputs = { self, nixpkgs, flake-utils, nix-filter }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
          filter = nix-filter.lib;
        in
        {
          packages = {
            wpilib = pkgs.callPackage ./nix/wpilib.nix { };
            readerwriterqueue = pkgs.callPackage ./nix/readerwriterqueue.nix { };
            safe = pkgs.callPackage ./nix/safe.nix { };
            tinyfsm = pkgs.callPackage ./nix/tinyfsm.nix { };

            deadeye-daemon =
              let
                gst = with pkgs.gst_all_1; [ gstreamer gst-plugins-base gst-plugins-good ];
                gstPlugins = pkgs.lib.makeSearchPathOutput "lib" "lib/gstreamer-1.0" gst;
              in
              pkgs.stdenv.mkDerivation
                {
                  pname = "deadeye-daemon";
                  version = "22.2.0";
                  src = filter {
                    root = self;
                    include = [ ./src ./cmake "CMakeLists.txt" "version.cmake" ];
                  };

                  nativeBuildInputs = with pkgs; [
                    cmake
                    pkg-config
                    makeWrapper
                  ];

                  buildInputs = with pkgs; [
                    catch2
                    libgpiod_1
                    nlohmann_json
                    spdlog.dev
                    systemd
                    self.packages.${system}.readerwriterqueue
                    self.packages.${system}.safe
                    self.packages.${system}.tinyfsm
                    self.packages.${system}.wpilib
                  ];

                  cmakeFlags = [
                    "-DDEADEYE_BUILD_TESTS=OFF"
                    "-DDEADEYE_BUILD_BENCHMARKS=OFF"
                    "-DDEADEYE_UNIT_ID=W"
                    "-DDEADEYE_NT_SERVER=10.27.67.2"
                    "-DDEADEYE_STREAM_ADDRESS=10.27.67.11"
                    "-DDEADEYE_CAMERA0_PIPELINE=deadeye::TargetListPipeline"
                    "-DDEADEYE_CAMERA1_PIPELINE=deadeye::UprightRectPipeline"
                  ];

                  postFixup = ''
                    wrapProgram "$out/bin/deadeyed" \
                      --prefix GST_PLUGIN_PATH : ${gstPlugins} \
                      --set LD_LIBRARY_PATH ${self.packages.${system}.wpilib}/wpilib/lib/
                  '';

                };

            default = self.packages.${system}.deadeye-daemon;

            dockerImage = pkgs.dockerTools.buildLayeredImage {
              name = "j3ff/deadeye-daemon";
              tag = "latest";

              config = {
                Cmd =
                  let
                    deadeyePkg = self.packages.${system}.deadeye-daemon;
                  in
                  [ "${deadeyePkg}/bin/deadeyed" ];
                ExposedPorts = {
                  "5805/tcp" = { };
                  "5806/tcp" = { };
                  "5807/tcp" = { };
                  "5808/tcp" = { };
                  "5809/tcp" = { };
                };
              };
            };
          };

          devShells.default = pkgs.mkShell {
            inputsFrom = [ self.packages.${system}.deadeye-daemon ];
            packages = with pkgs; [ just ];
          };
        });
}
