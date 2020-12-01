let
  pkgs_stable = import <nixos> { };
  pkgs = import <nixos-unstable> { sandbox = false; };

  eigen_trunk = pkgs.eigen.overrideAttrs (old: rec {
    version = "3.3.8";
    stdenv = pkgs.gcc10Stdenv;
    src = pkgs.fetchFromGitLab {
      owner = "libeigen";
      repo = "eigen";
      rev    = "${version}";
      sha256 = "1mrvcwnvwbwbm60ss7415sdgja6dq4bjcgvlc2zqmm6jycqhs9gn";
    };
    patches = [ ./eigen_include_dir.patch ];
  });

  fmt = pkgs.fmt.overrideAttrs(old: rec { 
    outputs = [ "out" ];

    cmakeFlags = [
      "-DBUILD_SHARED_LIBS=ON"
      "-DFMT_TEST=OFF"
      "-DFMT_CUDA_TEST=OFF"
      "-DFMT_FUZZ=OFF"
    ];
  });
in
  pkgs.gcc10Stdenv.mkDerivation {
    name = "aoc2020";
    hardeningDisable = [ "all" ]; 

    buildInputs = with pkgs; [
        # python environment for bindings and scripting
        gdb
        valgrind
        linuxPackages.perf
        cmake
        eigen_trunk
        gperftools
        jemalloc
        fmt
        clang_10
        hyperfine
      ];
    }
