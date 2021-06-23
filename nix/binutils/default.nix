{ pkgs, stdenv, buildPackages, fetchurl, ... }:
let
  sysroot = ../../sysroot;
in
  stdenv.mkDerivation rec {
    pname = "nightingale-binutils";
    version = "2.36.1";

    src = fetchurl {
      url = "https://ftp.gnu.org/gnu/binutils/binutils-2.36.1.tar.xz";
      sha256 = "1c7kn0qf1snlj98zd4zy9pbsjqpa99kmdwm053s3l69z6zgrw7g8";
    };

    dontUpdateAutotoolsGnuConfigScripts = true; # I am using a patched config.sub

    enableParallelBuilding = true;

    # from the upstream nix binutils build
    outputs = [ "out" "info" "man" ];
    depsBuildBuild = [ buildPackages.stdenv.cc ];
    nativeBuildInputs = with pkgs; [
      bison
      perl
      texinfo
    ];
    buildInputs = with pkgs; [ zlib gettext ];
    # /upstream

    patches = [
      ./nightingale-binutils-2.36.1.patch
    ];

    configureFlags = [
      "--target=x86_64-nightingale"
      "--disable-nls"
      "--disable-werror"
      "--with-sysroot=${sysroot}"
    ];
  }
