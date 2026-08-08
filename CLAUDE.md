# Arena PvP — contexte projet pour Claude

Jeu PvP-only qui reproduit les sensations des arènes 2v2/3v3 de WoW (sans PvE), en
**UE 5.8 épinglé** (ne pas monter de version), C++ + Gameplay Ability System.
Format moderne : manches BO3, kits resserrés 8-10 touches. IP originale (aucun nom/asset Blizzard).

**Lire avant de coder :** `docs/ARCHITECTURE.md` (décisions techniques + jalons M1→M7),
`docs/GDD.md` (design, kits, chiffres), `docs/checklists/` (tests par jalon).

## État actuel (mettre à jour à chaque jalon)

- **M1** (squelette, contrôles WoW, arène procédurale) : validé en jeu — déplacements approuvés par Quentin, ne pas dégrader ce feeling.
- **M2** (GAS, dégâts, tab-target, mannequins, UI) : sur la branche `claude/wow-pvp-arenas-game-bas9jd-m2`, en cours de test.
- **M3** (casts, interruptions, ressources, spell queue) : à venir.

## Workflow de branches

- Branche principale de dev : `claude/wow-pvp-arenas-game-bas9jd`.
- Chaque jalon se développe sur une branche dédiée (`...-m2`, `...-m3`) pendant que
  Quentin teste la précédente, puis est fusionné après validation.
- Commits en français, descriptifs. Jamais de push sur `main`.

## Compiler et tester (Windows local)

```powershell
& "C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" ArenaGameEditor Win64 Development -Project="<chemin>\ArenaGame.uproject" -WaitMutex
```

- UE compile en warnings-as-errors : attention au shadowing (C4458), variables inutilisées.
- Ne PAS builder la cible `ArenaGameServer` (nécessite un moteur source — prévu M7).
- Test en éditeur : PIE, et systématiquement **2 joueurs en Net Mode = Listen Server**
  pour tout ce qui touche au gameplay. Latence simulée : `Net PktLag=100`.
- Commandes console du jeu : `Arena.ReloadData` (recharge les CSV à chaud),
  `Arena.SetClass Warrior|Mage|Priest|Rogue`, `Arena.OpenGates`.

## Règles d'architecture (non négociables)

1. **Code-first absolu** : aucun asset binaire (.uasset/.umap) créé par Claude. Géométrie
   d'arène = `AArenaBuilder` depuis `Content/Data/ArenaLayout.csv` ; input = construit au
   runtime depuis `Config/DefaultArenaKeybinds.ini` ; UI = widgets UMG construits en C++ ;
   assets référencés via soft paths CSV avec fallback (jamais de crash sur asset manquant).
   Le travail éditeur humain se limite aux checklists de `docs/checklists/`.
2. **Data-driven** : tout l'équilibrage vit dans `Content/Data/*.csv` (structs dans
   `System/ArenaDataRows.h`, types simples uniquement — pas de FGameplayTag/enum importés).
   Un sort = une ligne d'`Abilities.csv`, servie par des classes C++ « verbes » génériques.
3. **GAS** : ASC sur le Character (mode `Full` joueurs, `Minimal` mannequins) ;
   `InitAbilityActorInfo` dans `PossessedBy` ET `OnRep_Controller`/`OnRep_PlayerState` ;
   GameplayEffects = classes C++ statiques + magnitudes **SetByCaller** — JAMAIS de
   `NewObject<UGameplayEffect>` au runtime (ne réplique pas) ; tags natifs uniquement
   (`ArenaGameplayTags.h/.cpp`, zéro ini) ; cooldowns par slot via le pool
   `Cooldown.Ability.SlotN` ; dégâts via ExecCalc + méta-attribut `Damage`.
4. **Réseau** : serveur autoritaire pour tout ; jamais de dégâts prédits client ;
   pas de rollback custom — la spell queue window (M3) gère la latence comme WoW.
5. **Jamais de root motion** (anims in-place only). Les includes du module sont relatifs
   à la racine de `Source/ArenaCore/` (déclarée dans le Build.cs).
6. Le GameMode vient de `GlobalDefaultGameMode` (DefaultEngine.ini) — les maps restent vides.

## Direction artistique (décidée)

- **Personnages : assets Paragon** (gratuits, licence limitée à Unreal — OK pour nous).
  Mapping classes → héros : Guerrier=Greystone, Mage=Gideon, Prêtre=Muriel, Voleur=Kallari.
  Les races (Valdris/Kargh) restent un choix de gameplay (raciaux) + variante de skin/teinte.
- **Décor : packs FANTASTIC de Tidal Flask** (stylisé hand-painted, l'esprit WoW).
  Pour l'arène : Battle Pack et/ou Dungeon Pack. Achat/import au M4. S'ils sont
  volumineux : même règle que Paragon (gitignore + soft paths).
- **RÈGLE CRITIQUE : les packs Paragon ne vont JAMAIS dans le repo git** (plusieurs Go/héros,
  le quota LFS exploserait). `Content/Paragon*` est gitignoré : Quentin les ajoute localement
  depuis Fab, les CSV les référencent en soft paths, et le fallback capsule garde le projet
  fonctionnel sans eux. Idem pour tout gros pack : gitignore + soft path + fallback.

## Contexte utilisateur

Quentin (solo dev, ex-joueur d'arène WoW) : clone unique dans
`C:\Users\malgu\Desktop\arena-gaming-wow 5.8`, clavier AZERTY, machine 24 cœurs/64 Go.
Il joue-teste chaque jalon et donne le retour de feeling — le feeling WoW prime sur tout.
