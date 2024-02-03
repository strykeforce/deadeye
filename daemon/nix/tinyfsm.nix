{ lib
, stdenv
, fetchFromGitHub
, cmake
}:
stdenv.mkDerivation rec {
  pname = "tinyfsm";
  version = "0.3.3";

  src = fetchFromGitHub {
    owner = "jhh";
    repo = pname;
    rev = "v${version}";
    sha256 = "sha256-to9nJG0BDXUwSqsGtID9VpsKjBKPFI+k0Btn75uMplU=";
  };

  outputs = [ "out" ];

  nativeBuildInputs = [ cmake ];

}


