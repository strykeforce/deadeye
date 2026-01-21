{ pkgs, perSystem }:

let
  nginxPort = "80";

  nginxConf = pkgs.writeText "nginx.conf" (
    builtins.replaceStrings
      [
        "%NGINX_PORT%"
        "%WEB_ROOT%"
        "%MIME_TYPES%"
      ]
      [
        "${nginxPort}"
        "${perSystem.self.web-static}"
        "${pkgs.nginx}/conf/mime.types"
      ]
      (builtins.readFile ../../web/nginx.conf)
  );
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
    Cmd = [
      "nginx"
      "-c"
      nginxConf
    ];
    ExposedPorts = {
      "${nginxPort}/tcp" = { };
    };
  };
}
