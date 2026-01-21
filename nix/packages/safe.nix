{
  pkgs,
}:
pkgs.stdenv.mkDerivation rec {
  pname = "safe";
  version = "1.0.1";

  src = pkgs.fetchFromGitHub {
    owner = "LouisCharlesC";
    repo = pname;
    rev = "v${version}";
    sha256 = "sha256-qFcdNr8dVTyRmpBQ/b7fSbDjJuE0fpPj3imFGosRVNY";
  };

  outputs = [ "out" ];

  nativeBuildInputs = [ pkgs.cmake ];

}
