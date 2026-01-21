{
  pkgs,
  perSystem,
}:
pkgs.dockerTools.buildLayeredImage {
  name = "j3ff/deadeye-daemon";
  tag = "latest";

  config = {
    Cmd =
      let
        deadeyePkg = perSystem.self.daemon-deadeyed;
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

  meta = {
    platforms = pkgs.lib.platforms.linux;
  };
}
