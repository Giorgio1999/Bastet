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
          chessPackages = chess.packages.x86_64-linux.default;
        in 
        {
          devShells.default = pkgs.mkShell {
            nativeBuildInputs = [
              chessPackages
              pkgs.gcc
              pkgs.cmake
            ];
            buildInputs = [
            ];

            CHESS_ROOT = "${chessPackages}";

            shellHook = ''
              echo chess shell with
              ${chessPackages}/bin/example_program
              exec zsh
            '';
          };
        }
      );
}
