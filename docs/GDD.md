# Game Design Document — Arena PvP

## Vision

Reproduire les **sensations des arènes 2v2/3v3 de WoW** sans la partie PvE, pour les joueurs
d'arène qui n'ont plus de jeu. Le cœur du feel : tab-target, GCD, incantations et interruptions,
contrôles avec rendements décroissants, jeu autour du soigneur, cooldown trading et pillar play.

**Modernisations assumées vs WoW :**
1. **Format en manches (BO3)** — manches de 2-3 min, victoire au premier à 2 manches,
   reset complet entre manches (PV, mana, cooldowns, DR). Pas de dampening.
2. **Kits resserrés** — 8-10 touches max par classe. Chaque bouton compte ; la profondeur
   vient des interactions, pas du nombre de binds.

**IP originale** : mécaniques inspirées, mais noms, univers et assets 100 % originaux
(assets tiers sous licence : Paragon, Tidal Flask — pas d'assets Blizzard).

## Direction artistique

- **Personnages : héros Paragon** (assets AAA gratuits d'Epic, animations de combat
  complètes). Mapping : Guerrier = Greystone, Mage = Gideon, Prêtre = Muriel,
  Voleur = Kallari. Les races Valdris/Kargh restent un choix de gameplay (raciaux)
  exprimé visuellement par une variante de skin/teinte — pas de morphologie dédiée
  tant qu'on utilise Paragon.
- **Décors : série FANTASTIC de Tidal Flask** (stylisé peint à la main, l'esprit
  visuel de WoW) — Battle/Dungeon Pack pour habiller l'arène au M4/M7.
- **VFX de sorts** : Paragon (chaque héros embarque les siens) + un pack Niagara
  stylisé par écoles si besoin de compléter.
- Les capsules restent la référence de lisibilité : tout habillage qui dégrade la
  lecture du combat (silhouettes, télégraphes de casts) sera revu.

## Règles de combat

| Système | Règle |
|---|---|
| GCD | 1,5 s, réduit par la hâte, plancher 0,75 s. Interruptions, trinket et raciaux hors GCD |
| Incantation | Barre de cast visible par tous ; portée vérifiée au début, **ligne de vue re-vérifiée à la fin** (le juke derrière un pilier fait échouer le sort) |
| Interruption | Ne fonctionne que sur un cast en cours → verrouille l'école du sort interrompu 4 s |
| CC & DR | 5 catégories (étourdissement, incapacité, racine, désorientation, silence). Par cible et par catégorie : 100 % → 50 % → 25 % → immunité, fenêtre de 18 s |
| Casse aux dégâts | Incapacité et désorientation cassent aux dégâts (racine : au-delà d'un seuil) |
| Ressources | Mana (régen passive), rage (générée en infligeant/subissant, décroît hors combat), énergie (régen rapide) + points de combo côté joueur |
| Trinket PvP | Brise tous les CC cassables, utilisable sous CC, 2 min, hors GCD — tout le monde l'a |
| Spell queue | Fenêtre de ~400 ms : l'input pressé juste avant la fin du GCD part dès qu'il expire (fluidité sous latence, comme WoW) |
| Réduction de soins | Frappe brutale : -25 % de soins reçus pendant 10 s — le moteur des victoires en mêlée |

## Classes (v1 — tous les chiffres dans `Content/Data/Abilities.csv`)

Base commune : ~20 000 PV (stats normalisées par des templates d'équipement), 7 m/s,
reculade à 45 %, trinket PvP.

### Guerrier — rage, mêlée pression
Charge (M7) · Frappe brutale (réduction de soins) · Heurtoir (filler) · Entaille aux jambes
(ralentissement) · Fracasse-crâne (interruption) · Coup assommant (étourdissement) · Cri de guerre (burst).

### Mage — mana, contrôle & burst à distance
Trait de givre (cast + ralentissement) · Lance de glace (bonus sur cible racinée) · Nova de givre
(racine AoE) · Mouton (incapacité, casse aux dégâts) · Contresort (interruption) · Transfert (M7) ·
Barrière de givre (absorption).

### Prêtre — mana, soigneur
Soins rapides · Rénovation (HoT) · Mot de pouvoir : Bouclier (absorption) · Châtiment (pression) ·
Dissipation (dispel allié / purge ennemie) · Cri psychique (désorientation AoE) · Suppression de la
douleur (-40 % dégâts subis sur un allié).

### Voleur — énergie + points de combo, burst furtif
Camouflage · Coup bas (étourdissement d'ouverture) · Attaque sournoise (générateur) · Éviscération
(finisher dégâts) · Aiguillon perfide (finisher étourdissement) · Coup de pied (interruption) · Sprint.

Chaque mécanique cœur (cast/interrupt, 5 catégories de DR, absorption, HoT, dispel, furtivité,
4 ressources, hors-GCD, réduction de soins) est couverte par au moins un sort : les kits servent
aussi de matrice de test des systèmes.

## Races

- **Valdris** (cités-bastions) — passif : +2 % stats secondaires ; actif **Volonté** :
  retire ralentissements et racines (1,5 min).
- **Kargh** (clans des steppes) — passif : -10 % de durée des étourdissements subis ;
  actif **Fureur ancestrale** : +10 % dégâts et soins pendant 10 s (1,5 min).

## Équipement

Templates normalisés par classe (`StatTemplates.csv`) : arme + 4 pièces + 2 trinkets définissent
PV, puissance, hâte, crit. **Puissance égale pour tous — le skill décide.** Trinket 1 : anti-CC
universel. Trinket 2 : choix mineur (proc de hâte vs stat plate). Post-MVP : trade-offs de builds,
cosmétiques.

## La map : Cercle de Kel'Vara

Arène ~52×42 m type Nagrand : sol plat, **2 piliers** cylindriques pour le pillar play,
**2 portes de départ** opposées avec alcôves (préparation 15 s), murs d'enceinte.
Générée procéduralement depuis `ArenaLayout.csv`. Zone d'entraînement avec 3 mannequins
(immobile / mobile / casteur) prévue en M2+.

## Boucle du premier jouable (1v1)

Menu → Entraînement (solo vs mannequins) ou Duel → héberger / rejoindre par IP → préparation
aux portes (15 s) → combat → mort = fin de manche → **BO3** → écran de score (dégâts/soins/CC)
→ revanche. Post-MVP : sessions Epic Online Services, 2v2/3v3, matchmaking coté (Glicko-2),
draft de talents entre manches, pings tactiques, spectateur/replays.
