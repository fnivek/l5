{ inputs, ... }:
{
  systems = [
    "x86_64-linux"
    "aarch64-linux"
    "x86_64-darwin"
    "aarch64-darwin"
  ];

  perSystem =
    { system, ... }:
    {
      _module.args.pkgs = import inputs.nixpkgs {
        inherit system;
        config.allowUnfree = true;
      };
    };

  # Shared CMake+Ninja builder. Pass pkgs explicitly so each perSystem call
  # gets the right platform's package set.
  _module.args.mkCmake =
    pkgs: name: src: extraBuildInputs:
    pkgs.stdenv.mkDerivation {
      pname = name;
      version = "0.1.0";
      src = src;
      nativeBuildInputs = with pkgs; [
        cmake
        ninja
      ];
      buildInputs = extraBuildInputs;
      cmakeFlags = [ "-GNinja" ];
    };
}
