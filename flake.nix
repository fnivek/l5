{
  description = "Flake to build everything in the monorepo.";

  inputs = {
    # Unstable packages from nixpkgs.
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    # Stable Nixpkgs from flake hub.
    flakehub.url = "https://flakehub.com/f/NixOS/nixpkgs/*";
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
        inputs.flakehub.lib.genAttrs supportedSystems (
          system:
          f {
            # Provides a system-specific, configured Nixpkgs
            pkgs-flake = import inputs.flakehub {
              inherit system;
              config.allowUnfree = true;
            };
            pkgs-nix = import inputs.nixpkgs {
              inherit system;
              config.allowUnfree = true;
            };
          }
        );
    in
    {
      # Development environments output by this flake
      devShells = forEachSupportedSystem (
        { pkgs-flake, pkgs-nix }:
        {
          # Run `nix develop` to activate this environment or `direnv allow` if you have direnv installed
          default = pkgs-flake.mkShell {
            # The Nix packages provided in the environment
            packages = with pkgs-flake; [
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
              pkgs-nix.godot
              gdtoolkit_4

              # Graphics
              pixelorama

              # Audio
              audacity

              # CMake
              cmake
            ];

            # Set any environment variables for your development environment
            env = { };

            # Add any shell logic you want executed when the environment is activated
            shellHook = ''
              printf "L4 *** J *** L5\n"
            '';
          };
        }
      );
    };
}
