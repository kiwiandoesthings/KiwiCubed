{
  description = "a game of some sort";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        buildInputs = with pkgs; [
          glfw
          xorg.libX11
          xorg.libxcb
          gettext
          nlohmann_json
          imgui
          glm
          libdecor
          libdecor.dev
        ];

        nativeBuildInputs = with pkgs; [
          cmake
          pkg-config
          git
        ];

        mkKiwi = buildType: pkgs.stdenv.mkDerivation {
          pname = "kiwicubed";
          version = "0.0.3-${buildType}";
          src = ./.;

          inherit nativeBuildInputs buildInputs;

          cmakeFlags = [
            "-DCMAKE_BUILD_TYPE=${buildType}"
          ];

          installPhase = ''
            runHook preInstall
            make install
            runHook postInstall
          '';
        };
      in {
        packages = rec {
          kiwicubedDebug = mkKiwi "Debug";
          kiwicubedRelease = mkKiwi "Release";

          # Default points to Release
          default = kiwicubedRelease;
        };

        apps = rec {
          debug = flake-utils.lib.mkApp {
            drv = self.packages.${system}.kiwicubedDebug;
            name = "kiwicubed";
          };

          release = flake-utils.lib.mkApp {
            drv = self.packages.${system}.kiwicubedRelease;
            name = "kiwicubed";
          };

          # Default runs Release
          default = release;
        };

        devshells.${system}.default = pkgs.mkShell {
          packages = nativeBuildInputs ++ buildInputs;
        };
      }
    );
}
