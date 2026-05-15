{ mkCmake, inputs, ... }:
{
  perSystem =
    { pkgs, ... }:
    {
      packages.behavior_trees = mkCmake pkgs "behavior_trees" (inputs.self + "/libs/behavior_trees") [ ];
    };
}
