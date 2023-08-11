{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem(system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
      in
      with pkgs;
      {
        devShells.default = mkShell rec {
          nativeBuildInputs = [
            pkg-config
            cmake
            clang
            gcc
          ];

          buildInputs = [
            stdenv.cc.cc.lib
          ];

          CMAKE_PREFIX_PATH = "/home/campbell/BlackBoxBio/pylon-rs/pylon_7_3_0/share/pylon/cmake";
        };
      }
    );
}
