{
  description = "Flake to build everything in the monorepo.";

  inputs = {
    # Unstable packages from nixpkgs.
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    # Used to import all nix files from a directory to use the dendritic pattern.
    import-tree.url = "github:vic/import-tree";
    # Construct flakes from reusable modules.
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs =
    { flake-parts, import-tree, ... }@inputs:
    flake-parts.lib.mkFlake { inherit inputs; } (import-tree ./nix);
}
