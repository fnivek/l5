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

      # Helper for providing system-specific attributes
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
    in
    {
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
        in
        {
          # Run `nix develop` to activate this environment or `direnv allow` if you have direnv installed
          default = pkgs.mkShell {
            # The Nix packages provided in the environment
            packages = with pkgs; [
              # Version control
              git
              gh

              # Language servers.
              helix
              nixfmt-rfc-style
              nil
              nixd
              vscode-langservers-extracted
              yaml-language-server
              # Ansible is broken see https://github.com/ansible/vscode-ansible/issues/1144
              # ansible-language-server
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

              # GoDot
              pkgs.godot
              gdtoolkit_4

              # Graphics
              pixelorama

              # Audio
              audacity

              # CMake
              cmake

              # Python
              py
              pyEnv
              poetry
            ];

            # Set any environment variables for your development environment
            env = { };

            # Add any shell logic you want executed when the environment is activated
            shellHook = ''
              export QT_PLUGIN_PATH="${pkgs.qt5.qtbase}/${pkgs.qt5.qtbase.qtPluginPrefix}"
              printf "L4 *** J *** L5\n"
            '';
          };
        }
      );
    };
}
