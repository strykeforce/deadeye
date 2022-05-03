{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
    n2n.url = "github:jhh/node2nix";
  };

  outputs = { self, nixpkgs, utils, n2n }:
    let
      version = "22.2.0";

      out = system:
        let pkgs = import nixpkgs
          {
            inherit system;
            overlays = [ n2n.overlays.${system} ];
          };
        in
        {

          devShells.default = pkgs.mkShell {
            buildInputs = with pkgs; [
              poetry
              nodejs-16_x
              node2nix
              nodePackages.http-server
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
          })
        ];
      };
    };
}
