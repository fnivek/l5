{
  description = "Flake to build everything in the monorepo.";

  inputs = {
    # Unstable packages from nixpkgs.
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs =
    { ... }@inputs:
    let
      supportedSystems = [
        "x86_64-linux" # 64-bit Intel/AMD Linux
        "aarch64-linux" # 64-bit ARM Linux
        "x86_64-darwin" # 64-bit Intel macOS
        "aarch64-darwin" # 64-bit ARM macOS
      ];

      forEachSupportedSystem =
        f:
        inputs.nixpkgs.lib.genAttrs supportedSystems (
          system:
          f {
            pkgs = import inputs.nixpkgs {
              inherit system;
              config.allowUnfree = true;
            };
          }
        );

      # Returns per-system packages. Adding a package here automatically
      # makes its nativeBuildInputs available in the devShell.
      mkRepoPkgs =
        pkgs:
        let
          mkCmake =
            name: src:
            pkgs.stdenv.mkDerivation {
              pname = name;
              version = "0.1.0";
              src = src;
              nativeBuildInputs = with pkgs; [
                cmake
                ninja
              ];
              cmakeFlags = [ "-GNinja" ];
            };
        in
        {
          behavior_trees = mkCmake "behavior_trees" ./libs/behavior_trees;
          l5 = mkCmake "l5" ./.;
        };
    in
    {
      packages = forEachSupportedSystem ({ pkgs }: mkRepoPkgs pkgs);

      # Development environments output by this flake
      devShells = forEachSupportedSystem (
        { pkgs }:
        let
          py = pkgs.python3;
          pyEnv = py.withPackages (
            ps: with ps; [
              pytest
              pytest-cov
              pytest-mock

              black
              flake8
              pylint
              pycodestyle
              ruff
              pyflakes
              yapf
              autopep8
              mccabe

              python-lsp-server

              sphinx
              sphinx-autodoc-typehints
              sphinx-rtd-theme

              click
              requests
              numpy
              matplotlib
              pyqt5
            ]
          );

          # Collect build deps from every repo package so the devShell
          # always has whatever is needed to build them locally.
          repoPkgsList = builtins.attrValues (mkRepoPkgs pkgs);
          repoNativeBuildInputs = builtins.concatMap (p: p.nativeBuildInputs or [ ]) repoPkgsList;
          repoBuildInputs = builtins.concatMap (p: p.buildInputs or [ ]) repoPkgsList;
        in
        {
          # Run `nix develop` to activate this environment or `direnv allow` if you have direnv installed
          default = pkgs.mkShell {
            packages =
              with pkgs;
              [
                # Version control
                git
                gh

                # Language servers
                nixfmt
                nil
                nixd
                vscode-langservers-extracted
                yaml-language-server
                taplo
                bash-language-server
                clang
                clang-tools
                cmake-language-server

                # Go task
                go-task

                # environment control
                direnv
                nix-direnv

                # Python
                py
                pyEnv
                poetry
              ]
              ++ repoNativeBuildInputs;

            buildInputs = repoBuildInputs;

            env = { };

            shellHook = ''
              # This Allows qt to work, we are using qt in the tools/plot_orbits.py in matplotlib.
              export QT_PLUGIN_PATH="${pkgs.qt5.qtbase}/${pkgs.qt5.qtbase.qtPluginPrefix}"
              printf "L4 *** J *** L5\n"
            '';
          };
        }
      );
    };
}
