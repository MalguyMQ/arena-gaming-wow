# Arena PvP (prototype)

Un jeu PvP-only qui reproduit les sensations des arènes 2v2/3v3 de WoW — sans le PvE.
Pour les joueurs d'arène orphelins : tab-target, GCD, incantations/interruptions, CC avec
rendements décroissants, pillar play. Format moderne en manches (BO3), kits resserrés.

**Moteur :** Unreal Engine 5.8 (C++ / Gameplay Ability System) · **Premier jouable :** duel 1v1 en ligne + mannequins.

## Démarrage rapide

1. Lire **[docs/SETUP.md](docs/SETUP.md)** (installation UE 5.5 + Visual Studio 2022).
2. Suivre la checklist du jalon en cours : **[docs/checklists/M1.md](docs/checklists/M1.md)**.

## Documents

| Doc | Contenu |
|---|---|
| [docs/GDD.md](docs/GDD.md) | Game design : classes, sorts, races, équipement, map, format BO3 |
| [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) | Architecture technique (GAS, réseau, data-driven) et jalons M1→M7 |
| [docs/SETUP.md](docs/SETUP.md) | Installation et boucle de travail |
| [docs/checklists/](docs/checklists/) | Checklist éditeur + tests d'acceptation par jalon |

## Principe de développement

Tout est **code et données** : le C++, les configs et les CSV d'équilibrage (`Content/Data/`)
sont la source de vérité. L'arène est générée procéduralement depuis `ArenaLayout.csv`,
l'input est construit au runtime depuis `Config/DefaultArenaKeybinds.ini`, et la commande
console `Arena.ReloadData` recharge les CSV à chaud sans recompiler. Le travail dans
l'éditeur Unreal se limite à quelques minutes par jalon, documentées dans les checklists.
