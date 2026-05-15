{ mkCmake, inputs, ... }:
{
  perSystem =
    { pkgs, ... }:
    {
      packages.l5 = mkCmake pkgs "l5" inputs.self [ ];
    };
}
