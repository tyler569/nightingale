{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  buildInputs = [
    (pkgs.callPackage ./nix/binutils {})
    (pkgs.callPackage ./nix/gcc {})
    pkgs.nasm
    pkgs.qemu
  ];
}

