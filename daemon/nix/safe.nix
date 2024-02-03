{ lib
, stdenv
, fetchFromGitHub
, cmake
}:
stdenv.mkDerivation rec {
  pname = "safe";
  version = "1.0.1";

  src = fetchFromGitHub {
    owner = "LouisCharlesC";
    repo = pname;
    rev = "v${version}";
    sha256 = "sha256-qFcdNr8dVTyRmpBQ/b7fSbDjJuE0fpPj3imFGosRVNY";
  };

  outputs = [ "out" ];

  nativeBuildInputs = [ cmake ];

}


