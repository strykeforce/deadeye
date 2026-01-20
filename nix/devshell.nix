{ pkgs, perSystem }:
perSystem.devshell.mkShell {
  packages = [
    pkgs.ninja
    pkgs.pkg-config
    pkgs.systemd
    pkgs.libgpiod_1
  ];

  packagesFrom = [
    perSystem.self.daemon-deadeyed
    perSystem.self.wpilib
  ];

  env = [
    {
      name = "DYLD_LIBRARY_PATH";
      prefix = "${perSystem.self.wpilib}/wpilib/lib/";
    }
    {
      name = "PKG_CONFIG_PATH";
      value = pkgs.lib.makeSearchPathOutput "dev" "lib/pkgconfig" [
        pkgs.systemd
        pkgs.libcap
        pkgs.libgpiod_1
      ];
    }
  ];

  commands =
    let
      daemon-image = "j3ff/deadeye-daemon";
    in
    [
      {
        name = "daemon:init";
        category = "daemon";
        help = "initialize the cmake build with sensible defaults";
        command = ''
          rm -rf $PRJ_ROOT/build
          cmake -GNinja -B$PRJ_ROOT/build -DDEADEYE_UNIT_ID=W -DDEADEYE_NT_SERVER=127.0.0.1 \
          -DDEADEYE_STREAM_ADDRESS=127.0.0.1 -DDEADEYE_CAMERA0_PIPELINE=deadeye::TargetListPipeline \
          -DDEADEYE_CAMERA1_PIPELINE=deadeye::UprightRectPipeline \
          $PRJ_ROOT/daemon
        '';
      }
      {
        name = "daemon:build";
        category = "daemon";
        help = "build the daemond executable";
        command = "ninja -C$PRJ_ROOT/build";
      }
      {
        name = "daemon:run";
        category = "daemon";
        help = "run the daemon";
        command = "$PRJ_ROOT/build/src/deadeyed";
      }
      {
        name = "daemon:test";
        category = "daemon";
        help = "run the daemon tests";
        command = "ctest --test-dir=$PRJ_ROOT/build";
      }
      {
        name = "daemon:image";
        category = "docker";
        help = "build and load the daemon docker image";
        command = ''
          nix build .#daemon-docker-image
          docker load < $PRJ_ROOT/result
          docker tag ${daemon-image}:latest ${daemon-image}:$(git rev-parse --short HEAD)
        '';
      }
    ];
}
