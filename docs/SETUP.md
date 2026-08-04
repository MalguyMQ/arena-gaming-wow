# Installation et boucle de travail

## Prérequis machine (Windows)

- Windows 10/11 64-bit
- 16 Go de RAM minimum (32 Go conseillés)
- GPU : GTX 1070 / RX 5700 ou mieux (le jeu est low-poly, ça suffit largement)
- ~150 Go de disque libre (moteur ~50 Go + projet + caches de compilation)

## Installation (une seule fois)

1. **Epic Games Launcher** → onglet Unreal Engine → Bibliothèque → installer **UE 5.5.x**
   (rester sur 5.5 : on ne change pas de version moteur avant le jalon M6).
2. **Visual Studio 2022 Community** (gratuit) avec le workload
   **« Développement de jeux avec C++ »** (Game development with C++).
   Vérifier que les composants « Windows 10/11 SDK » et « .NET SDK » sont cochés.
3. **Git + Git LFS** :
   ```
   winget install Git.Git
   git lfs install
   ```
4. Cloner le repo et se placer sur la branche de travail :
   ```
   git clone https://github.com/MalguyMQ/arena-gaming-wow.git
   cd arena-gaming-wow
   git checkout claude/wow-pvp-arenas-game-bas9jd
   ```

## Compiler et lancer

1. Clic droit sur `ArenaGame.uproject` → **Generate Visual Studio project files**.
   (Si l'option n'apparaît pas : ouvrir une fois le launcher Epic pour associer les .uproject.)
2. Ouvrir `ArenaGame.sln` dans Visual Studio → configuration **Development Editor | Win64** →
   **Build** (Ctrl+Maj+B). La première compilation prend 10-20 min.
3. Lancer avec F5 (ou double-clic sur le `.uproject` une fois compilé).
4. Suivre la checklist du jalon en cours dans `docs/checklists/`.

> Ne **pas** builder la cible `ArenaGameServer` : elle nécessite un moteur compilé
> depuis les sources (prévu au jalon M7). Le multijoueur du MVP passe par le listen server.

## La boucle de travail avec Claude

1. `git pull` sur la branche de travail — Claude y pousse le code de chaque jalon.
2. Regénérer les fichiers VS si de nouveaux fichiers C++ sont apparus, puis builder.
3. Dérouler la checklist `docs/checklists/Mx.md` (étapes éditeur + tests d'acceptation).
4. Remonter à Claude : les **erreurs de compilation** (copier-coller le texte complet),
   les échecs de checklist, et le **ressenti de jeu** (caméra, vitesse, feeling).
5. L'équilibrage se règle sans recompiler : éditer les CSV de `Content/Data/` puis taper
   `Arena.ReloadData` dans la console du jeu (touche `²` sur AZERTY).

Astuce : installer Claude Code en local (CLI/desktop) sur ce même repo permet à Claude
de compiler et corriger directement sur la machine — idéal pour le debug de compilation.

## Dépannage première compilation

- **« Missing Modules / Rebuild? » au double-clic du .uproject** : répondre Yes, ou
  builder depuis Visual Studio pour voir les vraies erreurs.
- **Erreur UnrealBuildTool / .NET** : réparer l'installation VS 2022, vérifier le workload C++.
- **L'éditeur s'ouvre sur un niveau vide sans arène** : normal tant que la map
  `Content/Maps/L_Arena` n'a pas été créée — voir `docs/checklists/M1.md`.
