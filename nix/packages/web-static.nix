{ pkgs }:
pkgs.buildNpmPackage {
  name = "deadeye-web";
  src = ../../web;
  npmDepsHash = "sha256-CBD2Yr/cDyPcl5JPQKu5uZjJJAtQmdVtWASyl8htzl0=";

  installPhase = ''
    runHook preInstall
    cp -r dist $out/
    runHook postInstall
  '';
}
