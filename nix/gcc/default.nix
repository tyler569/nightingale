{ buildPackages, fetchpatch, fetchurl, lib, pkgs, stdenv, ... }:
let
  ngBinutils = (pkgs.callPackage ../binutils {});
in
  pkgs.gcc11Stdenv.mkDerivation rec {
    pname = "nightingale-gcc";
    version = "11.1.0";

    src = fetchurl {
      url = "https://ftp.gnu.org/gnu/gcc/gcc-11.1.0/gcc-11.1.0.tar.xz";
      sha256 = "1pwxrjhsymv90xzh0x42cxfnmhjinf2lnrrf3hj5jq1rm2w6yjjc";
    };

    patches = [
      ./nightingale-gcc-11.1.0.patch
      # macos arm support
      (fetchpatch {
        url = "https://github.com/fxcoudert/gcc/compare/releases/gcc-11.1.0...gcc-11.1.0-arm-20210504.diff";
        sha256 = "sha256-JqCGJAfbOxSmkNyq49aFHteK/RFsCSLQrL9mzUCnaD0=";
      })
    ];

    # Needed because this breaks libmpc on ARM for some reason:
    # error: index must be an integer in range [-256, 255].
    #   ldr     x1, [x1, ___stack_chk_guard];momd
    NIX_CFLAGS_COMPILE = "-fno-stack-protector";

    # Needed because GCC does not build with -Werror=format-security
    hardeningDisable = [ "format" "pie" ];

    dontUpdateAutotoolsGnuConfigScripts = true; # I am using a patched config.sub

    outputs = [ "out" "man" "info" ];

    depsBuildBuild = [ buildPackages.stdenv.cc ];

    enableParallelBuilding = false;

    builder = ./builder.sh;
    staticCompiler = false;
    noSysDirs = true;
    crossStageStatic = true; # who knows

    nativeBuildInputs = with pkgs; [
      gmp
      mpfr
      libmpc
      libelf
    ];

    buildInputs = with pkgs; [
      zlib
      ngBinutils
    ];

    configureFlags = with pkgs; [
      # from upstream gcc build configure-flags.nix
      "--with-gmp-include=${gmp.dev}/include"
      "--with-gmp-lib=${gmp.out}/lib"
      "--with-mpfr-include=${mpfr.dev}/include"
      "--with-mpfr-lib=${mpfr.out}/lib"
      "--with-mpc=${libmpc}"
      "--with-libelf=${libelf}"

      "--target=x86_64-nightingale"
      "--disable-nls"
      "--disable-werror"
      "--enable-languages=c,c++"
      "--with-sysroot=../../sysroot"

      "--with-as=${ngBinutils}/bin/x86_64-nightingale-as"
      "--with-ld=${ngBinutils}/bin/x86_64-nightingale-ld"
    ];

    makeFlags = [ "all-gcc" "all-target-libgcc" ];
    installTargets = "install-gcc install-target-libgcc";

    meta = {
      homepage = "https://gcc.gnu.org/";
      license = lib.licenses.gpl3Plus;  # runtime support libraries are typically LGPLv3+
      description = "GNU Compiler Collection, version ${version} for the nightingale OS";
      platforms = lib.platforms.unix;
    };
  }
