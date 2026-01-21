{ pkgs, perSystem }:
perSystem.devshell.mkShell {

  packages = [
    pkgs.ninja
    pkgs.pkg-config
    pkgs.python3
    perSystem.uv2nix.uv-bin
  ]
  ++ pkgs.lib.optionals pkgs.stdenv.isLinux [
    pkgs.systemd
    pkgs.libgpiod_1
  ];

  packagesFrom = [
    perSystem.self.daemon-deadeyed
    perSystem.self.wpilib
  ];

  env =
    let
      libPathEnv = if pkgs.stdenv.isDarwin then "DYLD_LIBRARY_PATH" else "LD_LIBRARY_PATH";
    in
    [
      {
        name = libPathEnv;
        prefix = "${perSystem.self.wpilib}/wpilib/lib/";
      }
      {
        name = "UV_PYTHON_DOWNLOADS";
        value = "never";
      }
      {
        name = "PYTHONPATH";
        unset = true;
      }
    ]
    ++ pkgs.lib.optionals pkgs.stdenv.isLinux [
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
      admin-image = "j3ff/deadeye-admin";
      web-image = "j3ff/deadeye-web";
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
      {
        name = "admin:image";
        category = "docker";
        help = "build and load the admin docker image";
        command = ''
          nix build .#admin-docker-image
          docker load < $PRJ_ROOT/result
          docker tag ${admin-image}:latest ${admin-image}:$(git rev-parse --short HEAD)
        '';
      }
      {
        name = "web:image";
        category = "docker";
        help = "build and load the web docker image";
        command = ''
          nix build .#web-docker-image
          docker load < $PRJ_ROOT/result
          docker tag ${web-image}:latest ${web-image}:$(git rev-parse --short HEAD)
        '';
      }
    ];
}
