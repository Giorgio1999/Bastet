{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
    chess.url = "github:Giorgio1999/chess";
  };

  outputs = { self, nixpkgs, flake-utils, chess } :
    flake-utils.lib.eachDefaultSystem
      (system :
        let
          pkgs = import nixpkgs {
            inherit system;
          };
        in 

        devShells.default = pkgs.mkShell {
          nativeBuildInputs = [
            chess
          ];
          buildInputs = [
          ];

          shellHook = ''
            chess shell
          '';
        };
}
