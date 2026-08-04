# Architecture technique

UE **5.5.x** épinglé (pas de montée de version avant M6). C++ + GAS. Serveur autoritaire dès le
jour 1 — tout se teste en PIE multi-joueurs (listen server). Le projet est **maximalement
code-first et data-driven** : le dépôt ne contient que du texte (C++, ini, CSV, docs) ; les seuls
assets binaires (maps vides, feature pack, AnimBP) sont créés localement via les checklists.

## Décisions structurantes

- **ASC sur le Character** (pas le PlayerState) : pas de respawn en arène, et les mannequins ont
  besoin d'un ASC. `InitAbilityActorInfo` dans `PossessedBy()` **et** `OnRep_Controller()`.
- **Réplication GAS `Full`** pour les joueurs : les GameplayEffects répliquent à tous → les
  debuffs/buffs ennemis s'affichent sur les nameplates quasi gratuitement (cœur du feel arène).
  `Minimal` pour les mannequins. Échelle 2-6 joueurs : le coût de `Full` est négligeable.
- **GameplayEffects = classes C++** configurées au constructeur + magnitudes **SetByCaller**
  injectées depuis les CSV. Interdit : `NewObject<UGameplayEffect>` au runtime (ne réplique pas).
- **Tags natifs** dans `ArenaGameplayTags.h/.cpp` — zéro ini de tags.
- **GCD** : GE `Cooldown.GCD` à durée SetByCaller `clamp(1.5/(1+hâte), 0.75, 1.5)` ; flag
  hors-GCD par sort. **Spell queue window ~400 ms** dans l'ASC custom (M3) — GAS ne prédit pas
  l'expiration des cooldowns, on ne se bat pas contre ça, on file l'input comme WoW.
- **Cast bars** : les activations GAS ne répliquent pas aux proxys → struct répliquée
  `FArenaCastState` sur le personnage, interpolée contre `GetServerWorldTimeSeconds()`.
- **DR** : composant serveur par personnage, `TMap<catégorie, {niveau, fin de fenêtre 18 s}>`.
  Durée calculée AVANT application (×1 / ×0,5 / ×0,25 / immune) via SetByCaller.
- **Dégâts/soins** : pattern méta-attribut — ExecCalc capture AP/SP/crit/multiplicateurs,
  `PostGameplayEffectExecute` gère absorption → PV, mort, génération de rage, casse-aux-dégâts,
  cues de combat text. `HealingTakenMult` porte la réduction de soins.
- **Points de combo = attribut** (clamp 0-5), côté joueur.
- **Racines/stuns** : `GetMaxSpeed()` → 0 sous les tags CC (comportement WoW, léger rubber-band accepté).
- **Furtivité MVP** : tag → mesh caché côté ennemis, translucide pour soi. Durcissement
  net-relevancy (anti-cheat) en M7.
- **Input 100 % runtime** (`UArenaInputSetup`) : actions et mapping context construits en C++,
  touches lues dans `Config/DefaultArenaKeybinds.ini`. Zéro asset d'input.
- **Données** : CSV de `Content/Data/` chargés au runtime par `UArenaDataSubsystem`
  (`CreateTableFromCSVString`), console `Arena.ReloadData` pour recharger à chaud.
  Structures de lignes : types simples uniquement (`ArenaDataRows.h`).
- **Arène procédurale** : `AArenaBuilder` (répliqué, construit localement sur chaque machine
  depuis le CSV) avec les meshes `/Engine/BasicShapes/` + MID colorés. Lumières incluses.
  Les maps `.umap` restent vides.
- **UI** : widgets UMG construits en C++ (`WidgetTree->ConstructWidget`), police moteur.
  Cooldowns en overlay + décompte numérique (spirale radiale = matériau, M7).
- **Caméra/mouvement WoW** : `bUseControllerRotationYaw`, clic droit = mouselook, clic gauche =
  orbite, presser clic droit réaligne, reculade 45 %. **Jamais de root motion** (anims in-place).
- **GameMode via `GlobalDefaultGameMode`** (DefaultEngine.ini) — pas de World Settings à ouvrir.
- Cible `ArenaGameServer` committée jour 1 (discipline serveur), build dédié en M7.

## Arborescence Source/ArenaCore

```
ArenaGameplayTags.h/.cpp     Tags natifs
System/      AssetManager (InitGlobalData GAS), DataSubsystem (CSV), DataRows
AbilitySystem/  (M2+) ASC, AttributeSet, GameplayAbility de base,
                Abilities/ (verbes génériques), Effects/, Executions/, Tasks/
Combat/      (M2+) DRComponent, TargetingComponent, CueComponent, CombatStatics
Characters/  ArenaCharacter, ArenaCharacterMovementComponent, (M4) AnimInstance, TrainingDummy
Player/      PlayerController, PlayerState, InputSetup
Match/       MatchGameMode, GameState (M5 : phases + BO3), (M5) PlayerStart teams
Arena/       ArenaBuilder, (M5) ArenaGate
UI/          (M2+) HUD + widgets C++
```

## Jalons

| Jalon | Contenu | Critère de fin |
|---|---|---|
| **M1** | Squelette, contrôles WoW, arène procédurale, pipeline CSV | Déplacements/caméra OK en PIE 2 joueurs, `Arena.ReloadData` reconstruit l'arène |
| **M2** | GAS branché : attributs, GCD, dégâts (ExecCalc), tab-target, mannequin, frames/nameplates, action bar | On tape un mannequin avec les valeurs des CSV, synchro sur 2 clients |
| **M3** | Cast bars répliquées, interruptions + verrouillage d'école, ressources/coûts, portée/LoS/facing, CD par sort, spell queue, hâte | Juke derrière un pilier OK, kick verrouille l'école, fluide à 100 ms de lag simulé |
| **M4** | DR 5 catégories + trinket, casse-aux-dégâts, absorb/HoT/réduction de soins/dispel, combo+finishers, furtivité, les 4 kits, races, sélection de classe, icônes d'auras | Séquence DR 100/50/25/Immune observable, les 4 kits jouables vs mannequin |
| **M5** | Machine à phases : portes → combat → manche → **BO3** → victoire, reset entre manches, écran de score, revanche | BO3 complet en PIE 2 joueurs, tout se reset |
| **M6** | Menu héberger/rejoindre par IP, gestion erreurs réseau, packaging Windows, tests LAN + internet (Tailscale) | Un BO3 complet entre 2 machines distantes sans desync visible |
| **M7** | Anims (montages dynamiques), sons, spirale de CD, Charge/Transfert, EOS sessions, net-relevancy furtivité, serveur dédié | Feel complet, matchs sans ouverture de ports |

## Risques suivis

1. Prédiction GAS (pas d'expiration prédite des CD) → spell queue + tests `Net PktLag=100 PktLoss=2` à chaque jalon.
2. Dérive vers les assets éditeur → assets uniquement via checklists, soft paths + fallbacks.
3. NAT/CGNAT en IP directe → Tailscale pour les tests, EOS en M7, jamais de relay maison.
4. `CreateTableFromCSVString` potentiellement editor-only sur 5.5 → à vérifier au premier build M1 ; fallback parseur maison prévu.
5. Quotas Git LFS (~1 Go) → le repo reste ~99 % texte.
