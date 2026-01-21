{
  pkgs,
  perSystem,
}:
pkgs.dockerTools.buildLayeredImage {
  name = "j3ff/deadeye-admin";
  tag = "latest";

  config = {
    Cmd = [ "${perSystem.self.admin-venv}/bin/deadeye-server" ];

    ExposedPorts = {
      "5000/tcp" = { };
    };
  };
}
