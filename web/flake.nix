{
  description = "Deadeye vision system admin dashboard";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    nix-filter.url = "github:numtide/nix-filter";
  };

  outputs = { self, nixpkgs, flake-utils, nix-filter }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
          version = "22.2.0";
          nodeDependencies = (pkgs.callPackage config/default.nix { }).nodeDependencies;
          filter = nix-filter.lib;
        in
        {
          packages.nodeDependencies = nodeDependencies;

          packages.deadeye-web = pkgs.stdenv.mkDerivation {
            pname = "deadeye-web";
            inherit version;
            src = filter {
              root = self;
              include = [
                ./config
                "nginx.conf"
                "package.json"
                "package-lock.json"
                "postcss.config.js"
                ./public
                ./src
              ];
            };

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
              nginxConf = pkgs.writeText "nginx.conf" (builtins.replaceStrings [
                "%NGINX_PORT%"
                "%WEB_ROOT%"
                "%MIME_TYPES%"
              ] [
                "${nginxPort}"
                "${nginxWebRoot}"
                "${pkgs.nginx}/conf/mime.types"
              ]
                (builtins.readFile ./nginx.conf));
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
                ExposedPorts = { "${nginxPort}/tcp" = { }; };
              };
            };

          packages.default = self.packages.${system}.deadeye-web;

          devShells.default = pkgs.mkShell {
            packages = with pkgs; [ just nodejs_18 node2nix ];
          };
        });
}
