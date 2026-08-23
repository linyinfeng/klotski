{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" ];
      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f system);
    in
    {
      packages = forAllSystems (system:
        let pkgs = nixpkgs.legacyPackages.${system}; qt6 = pkgs.qt6; in
        {
          default = pkgs.stdenv.mkDerivation {
            pname = "klotski";
            version = "0.1.0";
            src = self;
            nativeBuildInputs = [ qt6.qmake qt6.qttools qt6.wrapQtAppsHook ];
            buildInputs = [ qt6.qtbase qt6.qtsvg ];
            configurePhase = ''
              runHook preConfigure
              mkdir build && cd build
              qmake ../klotski.pro "QT_TOOL.lrelease.binary=lrelease"
              runHook postConfigure
            '';

            installPhase = ''
              runHook preInstall
              install -Dm755 klotski "$out/bin/klotski"
              mkdir -p "$out/share/klotski"
              cp -r "$src/share"/{images,levels,solutions,help} "$out/share/klotski/"
              install -Dm644 "$src/desktop/application-icon/klotski_16.png"   "$out/share/icons/hicolor/16x16/apps/klotski.png"
              install -Dm644 "$src/desktop/application-icon/klotski_32.png"   "$out/share/icons/hicolor/32x32/apps/klotski.png"
              install -Dm644 "$src/desktop/application-icon/klotski_48.png"   "$out/share/icons/hicolor/48x48/apps/klotski.png"
              install -Dm644 "$src/desktop/application-icon/klotski_256.png"  "$out/share/icons/hicolor/256x256/apps/klotski.png"
              install -Dm644 "$src/desktop/application-icon/klotski_512.png"  "$out/share/icons/hicolor/512x512/apps/klotski.png"
              install -Dm644 "$src/desktop/klotski.desktop" "$out/share/applications/klotski.desktop"
              runHook postInstall
            '';

            meta = with pkgs.lib; {
              description = "Classic Klotski sliding block puzzle game";
              homepage = "https://github.com/linyinfeng/klotski";
              license = licenses.mit;
              mainProgram = "klotski";
            };
          };
        });

      apps = forAllSystems (system: {
        default = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/klotski";
        };
      });
    };
}
