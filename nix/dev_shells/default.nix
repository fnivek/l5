{ ... }:
{
  perSystem =
    { pkgs, config, ... }:
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
      devShells.default = pkgs.mkShell {
        # Pull nativeBuildInputs and buildInputs from every repo package so
        # the shell can build any of them locally without extra setup.
        inputsFrom = builtins.attrValues config.packages;

        packages = with pkgs; [
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

          # Environment control
          direnv
          nix-direnv

          # Python
          py
          pyEnv
          poetry

          # Build tools
          cmake
          emscripten

          # Native Linux build (SDL3; used by the linux target and clangd LSP)
          sdl3
        ];

        shellHook = ''
          # Allows Qt to work (used in tools/plot_orbits.py via matplotlib).
          export QT_PLUGIN_PATH="${pkgs.qt5.qtbase}/${pkgs.qt5.qtbase.qtPluginPrefix}"
          printf "L4 *** J *** L5\n"
        '';
      };
    };
}
