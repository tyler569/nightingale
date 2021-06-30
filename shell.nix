{ pkgs ? import <nixpkgs> {} }:
with pkgs; mkShell {
  buildInputs = [
    (callPackage ./nix/binutils {})
    (callPackage ./nix/gcc {})
    nasm
    qemu
  ];
}

