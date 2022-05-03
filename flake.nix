{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
    n2n.url = "github:jhh/node2nix";
  };

  outputs = { self, nixpkgs, utils, n2n }:
    let
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

        };
    in
    with utils.lib; eachSystem defaultSystems out // {

      nixosModules.admin = { config, lib, pkgs, ... }:
        with lib;
        let
          cfg = config.deadeye.admin;
        in
        {
          options.deadeye.admin = {
            enable = mkEnableOption "Enable the Deadeye admin service";

            port = mkOption {
              description = "Port the server listens on.";
              type = types.port;
              default = 5000;
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

            uploadDir = mkOption {
              description = "Directory used for image uploads.";
              type = types.str;
              default = "/tmp/deadeye";
            };
          };

          config = mkIf cfg.enable {
            systemd.tmpfiles.rules = [ "d ${cfg.uploadDir} 1777 root root 1d" ];

            systemd.services.deadeye-admin = {
              wantedBy = [ "multi-user.target" ];

              environment = {
                DEADEYE_ADMIN_PORT = "${toString cfg.port}";
                DEADEYE_NT_PORT = "${toString cfg.ntServerPort}";
                DEADEYE_NT_SERVER = cfg.ntServerAddress;
                DEADEYE_NT_WAIT_MS = "500";
                DEADEYE_UPLOAD_DIR = cfg.uploadDir;
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
          };
        };



      nixosConfigurations.admin = nixpkgs.lib.nixosSystem {
        system = "x86_64-linux";
        modules = [
          self.nixosModules.admin
          ({ config, pkgs, ... }: {
            # Only allow this to boot as a container
            boot.isContainer = true;
            networking.hostName = "deadeye-admin";

            # Allow nginx through the firewall
            networking.firewall.allowedTCPPorts = [ config.deadeye.admin.port ];

            deadeye.admin.enable = true;
            deadeye.admin.ntServerAddress = "192.168.1.30";
          })
        ];
      };
    };
}
