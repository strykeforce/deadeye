{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
          version = "22.2.0";
          nodeDependencies = (pkgs.callPackage config/default.nix { }).nodeDependencies;
        in
        {
          packages.nodeDependencies = nodeDependencies;

          packages.deadeye-web = pkgs.stdenv.mkDerivation {
            pname = "deadeye-web";
            inherit version;
            src = ./.;

            buildInputs = with pkgs; [ nodejs_18 ];

            phases = [ "unpackPhase" "buildPhase" ];

            buildPhase = ''
              ln -s ${nodeDependencies}/lib/node_modules ./node_modules
              export PATH="${nodeDependencies}/bin:$PATH"
              export NODE_ENV=production

              webpack  --config config/webpack.prod.js
              cp -r dist $out/
            '';
          };

          packages.dockerImage =
            let
              nginxPort = "80";
              nginxWebRoot = self.packages.${system}.deadeye-web;
              nginxConf = pkgs.writeText "nginx.conf" ''
                user nginx nginx;
                daemon off;
                error_log /dev/stdout info;
                pid /dev/null;
                events {}
                http {
                  access_log /dev/stdout;
                  server {
                    listen ${nginxPort};
                    index index.html;
                    location / {
                      root ${nginxWebRoot};
                    }
                  }
                }
              '';
            in
            pkgs.dockerTools.buildLayeredImage {
              name = "j3ff/deadeye-admin";
              tag = "latest";
              contents = with pkgs; [
                bashInteractive
                busybox
                nginx
              ];

              runAsRoot = ''
                #!${pkgs.runtimeShell}
                ${pkgs.dockerTools.shadowSetup}
                groupadd --system nginx
                useradd --system --gid nginx nginx
              '';

              extraCommands = ''
                # nginx still tries to read this directory even if error_log
                # directive is specifying another file :/
                mkdir -p var/log/nginx
                mkdir -p var/cache/nginx
              '';

              config = {
                Cmd = [ "nginx" "-c" nginxConf ];
                ExposedPorts = {
                  "${nginxPort}/tcp" = { };
                };
              };
            };

          packages.default = self.packages.${system}.deadeye-web;

          #
          devShells.default = pkgs.mkShell {
            # inputsFrom = [ self.packages.${system}.deadeye-admin ];
            packages = with pkgs; [ nodejs_18 node2nix ];
          };
        });
}
