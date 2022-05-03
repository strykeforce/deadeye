{
  description = "Webpack Tutorial";

  inputs.utils.url = "github:numtide/flake-utils";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs";
  inputs.n2n.url = "github:jhh/node2nix";

  outputs = { self, nixpkgs, utils, n2n }:
    utils.lib.eachDefaultSystem
      (system:
        let
          pkgs = import nixpkgs
            {
              inherit system;
              overlays = [ n2n.overlays.${system} ];
            };
          version = "1.0.0";
          nodeDependencies = (pkgs.callPackage ./config/default.nix { }).nodeDependencies;
        in
        {
          packages.default = pkgs.stdenv.mkDerivation {
            pname = "webpack-tutorial";
            inherit version;
            src = ./.;
            buildInputs = with pkgs; [ nodejs-16_x ];

            phases = "buildPhase";

            # ln -s $src/public ./public
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

          devShells.default = with pkgs; mkShell {
            packages = [
              nodejs-16_x
              node2nix
              nodePackages.http-server
            ];
          };
        }) // {
      nixosModules.default = { config, lib, pkgs, ... }:
        with lib;
        let
          cfg = config.deadeye.web;
          webPkg = self.packages.${pkgs.system}.default;
        in
        {
          options.deadeye.web = {
            enable = mkEnableOption "Enable the Deadeye web console";

            domain = mkOption rec {
              type = types.str;
              default = "deadeye.strykeforce.org";
              example = default;
              description = "The domain name for Deadeye web console";
            };
          };

          config = mkIf cfg.enable {
            services.nginx.enable = true;
            services.nginx.recommendedProxySettings = true;
            services.nginx.virtualHosts.${cfg.domain} = {
              default = true;

              locations = {
                "/" = {
                  root = "${webPkg}";
                  index = "index.html index.htm";
                  tryFiles = "$uri $uri/ /index.html";
                };

                "/socket.io/" = {
                  proxyPass = "http://127.0.0.1:5000";
                  proxyWebsockets = true;
                };

                "/upload" = {
                  proxyPass = "http://127.0.0.1:5000";
                };

                "/stream/0/" = { proxyPass = "http://127.0.0.1:5805/stream.mjpg"; };
                "/stream/1/" = { proxyPass = "http://127.0.0.1:5806/stream.mjpg"; };
                "/stream/2/" = { proxyPass = "http://127.0.0.1:5807/stream.mjpg"; };
                "/stream/3/" = { proxyPass = "http://127.0.0.1:5808/stream.mjpg"; };
                "/stream/4/" = { proxyPass = "http://127.0.0.1:5809/stream.mjpg"; };
              };
            };
          };
        };

      nixosConfigurations.container = nixpkgs.lib.nixosSystem {
        system = "x86_64-linux";
        modules = [
          self.nixosModules.default
          ({ pkgs, ... }: {
            # Only allow this to boot as a container
            boot.isContainer = true;
            networking.hostName = "deadeye";

            # Allow nginx through the firewall
            networking.firewall.allowedTCPPorts = [ 80 ];

            deadeye.web.enable = true;
          })
        ];
      };

    };
}
