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
                user nobody nobody;
                daemon off;
                error_log /dev/stdout info;
                pid /dev/null;
                events {}
                http {
                  include ${pkgs.nginx}/conf/mime.types;
                  access_log /dev/stdout;
                  server {
                    listen ${nginxPort};

                    location / {
                      root ${nginxWebRoot};
                      index index.html;
                      try_files $uri $uri/ /index.html;
                    }

                    location /socket.io/ {
                      proxy_set_header Upgrade $http_upgrade;
                      proxy_set_header Connection "Upgrade";
                      proxy_http_version 1.1;
                      proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
                      proxy_set_header Host $host;
                      proxy_pass http://admin:5000;
                    }

                    location /upload {
                      proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
                      proxy_set_header Host $host;
                      proxy_pass http://admin:5000;
                    }

                    location /stream/0/ {
                      proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
                      proxy_set_header Host $host;
                      proxy_pass http://daemon:5805/stream.mjpg;
                    }

                    location /stream/1/ {
                      proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
                      proxy_set_header Host $host;
                      proxy_pass http://daemon:5806/stream.mjpg;
                    }

                    location /stream/2/ {
                      proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
                      proxy_set_header Host $host;
                      proxy_pass http://daemon:5807/stream.mjpg;
                    }

                    location /stream/3/ {
                      proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
                      proxy_set_header Host $host;
                      proxy_pass http://daemon:5808/stream.mjpg;
                    }

                    location /stream/4/ {
                      proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
                      proxy_set_header Host $host;
                      proxy_pass http://daemon:5809/stream.mjpg;
                    }
                  }
                }
              '';
            in
            pkgs.dockerTools.buildLayeredImage {
              name = "j3ff/deadeye-web";
              tag = "latest";
              contents = with pkgs; [
                fakeNss
                nginx
              ];

              extraCommands = ''
                mkdir -p var/log/nginx
                mkdir -p var/cache/nginx
                mkdir tmp
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
            packages = with pkgs; [ just nodejs_18 node2nix ];
          };
        });
}
