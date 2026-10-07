# Saari et Maku — synthèse et enseignements des captures Gaussian Splatting

État au 25 septembre 2026. Exporteur C++ dans `cpp-offline`.

Ce document réunit le retour d'expérience de Saari et son application à Maku.
Il distingue les résultats rapportés par l'utilisateur, les contrôles de l'exporteur
et les hypothèses restant à évaluer après entraînement dans Postshot.

## Enseignements de Saari

### Du parcours de la démo à un parcours de capture

Le premier essai utilisait une petite série d'images rendues le long de la caméra
originale. Les captures du 19 septembre montrent une île reconnaissable, avec des
nappes et des étirements visibles depuis des vues extérieures au trajet. Elles
constituent une observation qualitative : elles ne suffisent pas à attribuer
chaque artefact au mouvement de Klunssi, aux poses estimées ou au manque de vues.
Voir le [document de conception Saari](forward-saari-gsplat-capture-approach.md).

Le jeu dédié a ensuite combiné une couverture de l'île et des objets alentour,
un état figé de la scène et l'export des caméras connues. L'utilisateur a indiqué
que le résultat fonctionnait « hyper bien », ce qui a motivé l'extension à Maku.
Ce retour soutient l'intérêt de l'approche complète ; aucune comparaison contrôlée
n'a encore isolé le gain propre à chaque changement.

**Enseignement : un parcours conçu pour montrer une scène dans la démo ne garantit
pas sa couverture depuis d'autres points de vue.** Pour une exploration plus libre,
il faut prévoir des observations complémentaires des surfaces et des occultations.
Dans Saari, cela a conduit à une hémisphère autour d'un ensemble délimité ; dans
Maku, à une deuxième hauteur de parcours dans un terrain répété et très embrumé.

### Séparer l'état de la scène des déplacements de capture

Saari fige son temps local à **30 secondes** par défaut. La position et la rotation
de Klunssi, ainsi que les transformations de son reflet, restent cohérentes entre
les prises. Les chocs scriptés sont désactivés. La caméra peut alors se déplacer
indépendamment de l'animation ; une même pose au même temps figé reproduit le même PNG.

Le gel concerne l'état animé. Le reflet, les matériaux et l'environment mapping
continuent de dépendre de la vue selon les règles du moteur. Une scène statique
peut donc conserver des variations d'apparence difficiles à apprendre.

**Application à Maku :** son terrain est déjà statique, mais le mélange avec
l'image précédente et les effets de rémanence doivent être désactivés pour qu'une
image corresponde à une seule caméra. La reproduction des PNG aux mêmes poses,
malgré une densité de capture différente, est contrôlée dans les tests.

### Définir les vues autorisées et le niveau de détail recherché

L'hémisphère de Saari englobe l'île, Meditate et Klunssi dans sa pose figée.
Les caméras restent au moins **2 unités au-dessus du plan de la mer**, y compris
lors d'une relecture de parcours. Les reflets sont observés depuis le dessus ;
leurs positions virtuelles sous l'eau n'agrandissent pas l'enveloppe à capturer.
Cette contrainte définit le domaine dans lequel la reconstruction est recherchée.

La couverture générale est complétée par un **focus sur Meditate**, avec un FOV
horizontal de **35°**, contre environ **80,21°** pour l'île. Des vues de liaison
gardent un contexte commun. Le jeu exporté comprend 225 vues `island`, 15 vues
`bridge` et 60 vues `meditate`, avant séparation entraînement/validation.

**Enseignement : couvrir un objet dans l'image ne garantit pas assez de pixels
pour ses détails.** Le focus associe cadrage et vues complémentaires, tout en
enregistrant la focale effective. Les PNG sont rendus directement en 1024 × 768 ;
la résolution des textures et de la géométrie d'origine demeure inchangée.
Pour Maku, ce principe motive les pistes de cadrage vertical et de vues ciblées,
sans présumer qu'une hémisphère serait adaptée au brouillard dense.

### Exporter des observations conformes au rendu

Saari a établi le socle repris par Maku : PNG sans perte, paramètres internes et
poses effectives des caméras, points issus de la géométrie connue, puis observations
réciproques au format COLMAP. Le CSV conserve les intentions du parcours ; les
fichiers COLMAP décrivent les poses et la projection réellement employées.

Deux points de contrôle sont essentiels :

- La conversion de repère s'applique aux caméras **et** au nuage. Les tests de
  reprojection vérifient les orientations, les focales et l'absence d'inversion.
- La visibilité suit le tri et la composition du moteur. Être projeté dans le
  cadre ne suffit pas : les surfaces masquées, le ciel et les pixels de réflexion
  composée ne fournissent pas artificiellement des observations de terrain opaque.

Le temps figé, le parcours et les réglages sont archivés. Saari permet aussi de
modifier puis rejouer `camera_path.csv` avec le même temps et la même résolution.
Dans Maku, le CSV documente actuellement les parcours sans fonction de relecture.

### Résultat archivé et portée des vérifications

Le [manifeste du jeu Saari](../cpp-offline/output-saari-gsplat/capture.json) contient
**300 PNG**, dont **270 d'entraînement et 30 de validation**, et **16 547 points
initiaux** : 3 730 associés à Meditate et 4 321 à Klunssi. Ces nombres décrivent
l'export géométrique, pas le nombre de gaussiennes du modèle entraîné.

Les tests couvrent notamment les PNG, les poses, les pistes d'observation, la
relecture CSV, le gel de la scène et la séparation des vues réservées. Les couleurs
et la sélection des points n'utilisent que les observations d'entraînement.
Le rendu Saari classique a également été comparé avant/après sur 50 images
espacées d'une seconde, avec le WAV et le manifeste, sans différence d'octets.

La validation de l'export et le retour visuel positif sont deux résultats distincts.
Les mesures comparatives sur les silhouettes, les trous, les reflets ou le détail
de Meditate restent à établir. Le blocage historique du test Postshot en ligne de
commande par la licence Studio concerne cette tentative d'automatisation ; il ne
remplace pas le retour utilisateur obtenu avec son propre processus.

## Ce qui se transpose de Saari à Maku

| Sujet | Saari | Maku | Enseignement commun |
| --- | --- | --- | --- |
| Cohérence entre images | Klunssi et son reflet figés à un temps local choisi | Terrain statique ; effets dépendant des images précédentes désactivés | Une capture doit correspondre à un état reproductible et à une caméra déterminée |
| Couverture | Hémisphère englobante, au-dessus de la mer | Parcours original, copie à +H/4 et grille au-dessus du terrain | Adapter les points de vue aux surfaces recherchées et aux limites du rendu |
| Détails | Focus Meditate et vues de liaison | Grille avec vues obliques et verticales pour les sommets | Évaluer séparément les zones importantes, au-delà de l'impression globale |
| Caméras et points | Poses exactes et visibilité selon la composition native | Même exporteur COLMAP, terrain distingué par tuile | Vérifier ensemble images, projection, repère et observations |
| Apparence | Reflets, environment mapping, textures affines et brouillard conservés | Textures affines et brouillard dense conservés | Des poses exactes ne suppriment pas les variations d'apparence du moteur |
| Évaluation | Vues réservées, relecture et navigation autour de l'île | Vues réservées aux deux hauteurs et approche/éloignement des reliefs | Distinguer validité des fichiers, qualité des vues entraînées et comportement hors parcours |

Les deux expériences produisent une trace spatiale de l'apparence dans un état et
un domaine de vues donnés. Conserver le code, les assets, les PNG, les caméras et
les paramètres reste nécessaire pour retrouver l'animation, expliquer les écarts
et rejouer les captures. Pour les comparaisons futures, faire varier un facteur à
la fois aidera à séparer les effets du parcours, du gel temporel, des poses et du
brouillard.

## Maku : contexte et décision retenue

Après l'expérience [Saari](forward-saari-gsplat-capture-approach.md), Maku utilise
le même principe : rendre des PNG dans la version C++ offline et exporter les
caméras connues ainsi qu'un nuage initial au format COLMAP pour Postshot.
Le premier jeu conserve le parcours de la démo, avec un FOV horizontal élargi.

Le retour utilisateur sur le premier GSplat est **très positif**, avec deux
limites visibles : des sommets « décapités » et des morceaux de décor manquants.
Une couverture insuffisante est une explication plausible, à distinguer d'un
défaut de géométrie du moteur ou d'une limite de l'entraînement. La hauteur de
caméra, le cadrage, les occlusions et le brouillard peuvent tous intervenir ;
la cause précise des trous n'a pas encore été mesurée.

La deuxième itération a ajouté **le même parcours à +H/4**, en conservant le
brouillard. La troisième ajoute une **grille horizontale au-dessus du terrain**,
sur une emprise XY égale à 150 % de celle du parcours nominal. Les trois
couvertures sont réunies dans **un seul jeu d'images et un seul modèle COLMAP**.
Le brouillard original reste actif dans les trois cas.

| Élément | État actuel |
| --- | --- |
| Caméras | Parcours original, copie à +87,785 unités suivant Z, puis grille de 13 × 6 stations |
| Orientation | Conservée pour les deux parcours ; 17 orientations par station de grille |
| Rendu | 1024 × 512, FOV horizontal 80°, brouillard original |
| Export combiné | 1 926 PNG : 1 734 pour l'entraînement et 192 pour la validation |
| Nuage initial | 103 029 points ; 27 467 partagés entre grille et parcours |
| Dossier | `cpp-offline/output-maku-gsplat-grid` |
| Validation | Tests des poses et de l'export ; gain visuel du GSplat avec grille à évaluer dans Postshot |

## Parcours et rendu de Maku

Le mode `maku-gsplat` reprend le parcours de caméra du rendu C++ de la démo :
positions et cibles dans `original/forward/asses/vuori5.ase`, interpolation
Catmull-Rom bouclée existante et commandes `go` / `speed` du script Maku.
Il conserve les changements de sens et les coupes entre les cinq portions.
Aucun passage sur une hémisphère n'est ajouté.

Les deux premières passes restent le parcours original, puis le même parcours
remonté de **H/4**. H est la hauteur du terrain,
calculée depuis les valeurs réellement utilisées dans la heightmap avec l'échelle
du moteur. Les altitudes vont d'environ 110,58 à 461,72 unités : H vaut donc
351,14 et le décalage vaut **87,785 unités**.

La caméra et sa cible sont translatées ensemble suivant Z ; la base d'orientation
originale est conservée exactement. Le FOV, les instants et le brouillard sont
identiques pour les deux passes. Il y a par défaut **300 vues par parcours**, soit
**600 PNG avant l'ajout de la grille**. Le terrain n'est pas déplacé : les points
communs relient les parcours et la grille dans un unique modèle COLMAP.

La capture couvre toute la scène, de `0x0D00` inclus à `0x1000` exclu, soit environ
24,407 secondes. Les instants des commandes sont résolus par le lecteur XM C++
existant, avec l'horloge de référence à 22050 Hz. Les vues sont réparties
uniformément dans cet intervalle, au pas de cette horloge. Le nombre de vues
modifie la densité de capture, sans modifier la trajectoire ou sa durée.
Une coupe sélectionne directement la portion suivante, sans raccord inventé.

| Début XM | Temps local approximatif | `go` (secondes de piste ASE) | `speed` |
| --- | ---: | ---: | ---: |
| `0x0D00` | 0,000 s | 160,5 | -3 |
| `0x0E00` | 8,136 s | 25,5 | 2 |
| `0x0E20` | 12,203 s | 0 | 2,5 |
| `0x0F00` | 16,271 s | 42,5 | -2 |
| `0x0F20` | 20,339 s | 55,5 | 4 |

Le FOV horizontal passe de **1,2 radian (68,75°)** à **80°** par défaut.
Le ratio natif 2:1 est conservé avec un rendu direct en **1024 × 512** pixels.
La focale exportée correspond exactement à celle utilisée pour rasteriser les PNG.

Le brouillard dense, les textures et le terrain restent ceux du rendu C++.
Pour la capture uniquement, le mélange avec l'image précédente, les effets `ksor`
et les chocs sont désactivés : chaque image dépend uniquement de sa caméra.
La géométrie du terrain est statique ; il n'y a pas d'objet animé à figer comme
Klunssi dans Saari. Le rendu classique `--sequence maku` conserve ses effets.

## Troisième itération : grille au-dessus du décor

Maku est un terrain répété par tuiles, sans bounding box XY finie pour l'ensemble
du décor. La zone de capture est donc définie par les **positions de la caméra
nominale effectivement jouées dans la démo**, sans inclure les cibles ni la
trajectoire surélevée. La mesure utilise un échantillonnage fixe à **50 Hz**, les
deux côtés de chaque coupe et le dernier échantillon de la scène. Elle ne dépend
pas du nombre de PNG demandé. C'est une enveloppe échantillonnée du parcours,
pas un calcul analytique des extrema des splines.

L'emprise mesurée est environ `X = [−1,826 ; 595,358]` et
`Y = [338,844 ; 571,830]`. **150 % signifie multiplier chaque dimension par 1,5**
autour du même centre : 25 % de la dimension originale sont ajoutés de chaque
côté. La surface est donc multipliée par 2,25. La grille couvre environ
**895,78 × 349,48 unités**, avec `X = [−151,122 ; 744,653]` et
`Y = [280,598 ; 630,076]`.

Les stations comprennent les quatre bords du rectangle. Avec un espacement
maximal de **75 unités**, cela donne **13 colonnes et 6 lignes**, soit 78 stations.
Les pas effectifs sont environ 74,65 et 69,90 unités. Le parcours alterne le sens
des lignes pour relier les stations voisines.

Toutes les stations sont placées sur le même plan :
`Z_grille = Z_max_terrain + H/8`, soit **505,6125 unités**. La marge au-dessus du
sommet vaut **43,8925 unités**. L'altitude ne suit pas le maximum Z des caméras,
qui peut être plus haut ; elle dépend du terrain. La marge donne de la place
pour les vues obliques tout en limitant la distance ajoutée dans le brouillard.

Chaque station produit **17 vues**, avec le même FOV de 80° :

- huit azimuts espacés de 45°, inclinés de 30° vers le bas ;
- les huit mêmes azimuts, inclinés de 60° vers le bas ;
- une vue verticale descendante, sans répéter huit images identiques au nadir.

La grille ajoute **1 326 images**, indépendamment de `--frames`. Les prises depuis
une même station partagent un centre ; les déplacements entre stations apportent
des points de vue supplémentaires. Les poses sont connues directement du moteur.
Les vues de validation sont prélevées toutes les dix prises, en recommençant le
comptage au début de la grille.

Le brouillard et le seuil de profondeur caméra de 200 unités sont conservés.
La grille peut observer les sommets et les versants depuis le haut, mais elle
ne rend pas visibles les vallées masquées par le brouillard. Un point n'entre
dans le nuage initial que s'il est réellement visible dans au moins deux vues
d'entraînement. Le manifeste indique les vues et les observations même si leur
apport est faible ; aucune image n'est automatiquement supprimée.

## Utilisation

Depuis la racine du dépôt, après compilation :

```powershell
cmake --build cpp-offline/build --config Release
cpp-offline/build/Release/forward-export.exe --sequence maku-gsplat --output cpp-offline/output-maku-gsplat-grid
```

Pour un autre essai, choisir un **nouveau dossier** :

```powershell
cpp-offline/build/Release/forward-export.exe --sequence maku-gsplat --frames 600 --gsplat-fov 85 --output cpp-offline/output-maku-gsplat-600-fov85
```

| Option | Valeur par défaut / rôle |
| --- | --- |
| `--frames` | 300 vues par parcours, entre 12 et 5000 ; la grille s'ajoute indépendamment |
| `--width`, `--height` | 1024 et 512, entre 16 et 4096 chacun |
| `--gsplat-fov` | FOV horizontal de 80°, entre 10° et 120° |
| `--gsplat-height-fraction` | 0,25 : décalage de H/4 ; valeurs entre 0 et 1 ; 0 désactive la deuxième passe |
| `--gsplat-grid-scale` | 1,5 : dimensions XY à 150 % ; valeurs de 1 à 3 ; 0 désactive la grille |
| `--gsplat-grid-spacing` | 75 : espacement maximal entre stations, entre 10 et 1000 unités |
| `--gsplat-grid-clearance` | 0 : marge automatique H/8 au-dessus du terrain ; valeur positive jusqu'à 1000 pour imposer la marge |
| `--gsplat-validation-every` | 10 : une vue sur dix de chaque passe réservée à la validation ; 0 : toutes pour l'entraînement |

`--fps` et `--sample-rate` ne changent pas ce jeu de vues fixes et aucun fichier
audio n'est créé. Les options de temps figé, de rayon et de relecture CSV restent
propres à Saari et Feta. Les options de fin musicale et de post-roll ne s'appliquent pas
à ce mode, qui couvre toujours la scène complète.

`--gsplat-grid-scale 0` reproduit les deux parcours de la deuxième itération.
Ajouter `--gsplat-height-fraction 0` permet de retrouver uniquement le parcours
nominal. Une grille dépassant 10 000 vues est refusée avant création du dossier :
augmenter le pas ou réduire l'emprise dans ce cas.

## Sortie pour Postshot

Le format reprend celui de Saari, avec un exporteur COLMAP commun :

- `images/` : les PNG d'entraînement des parcours et de la grille (1 734 par défaut).
- `sparse/cameras.txt` : modèle `PINHOLE`, taille, `fx = fy`, `cx`, `cy`.
- `sparse/images.txt` : poses monde-vers-caméra, quaternion Hamilton `qw qx qy qz`,
  translation, nom exact du PNG et observations 2D.
- `sparse/points3D.txt` : points colorés et pistes d'observation réciproques.
- `validation/images/` et `validation/sparse/` : les vues réservées (192 par défaut).
- `camera_path.csv` : positions, cibles, FOV, groupe et temps dans le repère natif.
- `manifest.csv` : lien entre image, pose, groupe, split et temps local.
- `capture.json` : paramètres, instants des segments, compteurs et marqueur de fin.
- `IMPORT.txt` : rappel des instructions d'import.

Les numéros d'image restent uniques : par défaut, `frame_000000.png` à
`frame_000299.png` pour le parcours original, puis `frame_000300.png` à
`frame_000599.png` pour la passe surélevée. Chaque dixième vue est rangée dans
`validation/images/` au lieu de `images/`. Les parcours et la grille s'importent ensemble ;
aucun alignement de deux reconstructions séparées n'est nécessaire.

Dans Postshot, importer **`images/` et les trois fichiers de `sparse/` ensemble**,
comme pour Saari. Garder `validation/` à part et éviter d'importer la racine entière.

Le repère COLMAP applique `x_export = -x_natif`, en conservant Y et Z ; Z reste
vertical. Cette conversion commune à Saari fournit une rotation propre tout en
préservant la projection du moteur. Le quaternion et la translation ne sont pas
une position de caméra brute : le centre exporté vérifie `C = -Rᵀ t`.

Les colonnes CSV sont :

```text
px,py,pz,tx,ty,tz,hfov_degrees,group,scene_time_seconds,track_time_seconds,roll_radians,pass,height_offset,grid_x,grid_y,azimuth_degrees,downward_degrees
```

Les huit premières sont les mêmes que pour Saari. `group` identifie la portion
du script (`maku_d00`, etc.). Le temps local et le temps de piste ASE sont distincts,
à cause des `go` et des vitesses signées. Le roll est nul dans ce script.
`pass` distingue `original`, `raised` et `grid`, et `height_offset` donne la translation
verticale appliquée, en unités du moteur. Ces deux colonnes sont également dans
le manifeste. `capture.json` enregistre les bornes verticales, la fraction de
hauteur et les passes. Le temps de scène repart de zéro pour la deuxième passe.
Pour la grille, `group` vaut `maku_grid`, le temps de scène vaut 0 (terrain
statique), et `track_time_seconds` ainsi que `height_offset` restent vides.
Les quatre dernières colonnes décrivent la station et les angles ; elles sont
vides pour les parcours et figurent aussi dans le manifeste. Le bloc `grid` de
`capture.json` archive les bornes mesurées, l'emprise agrandie, la hauteur, le pas,
les dimensions, les points partagés et le nombre de vues d'entraînement sans
point retenu.
Ce CSV sert à l'inspection et à la réutilisation des coordonnées, sans fonction
de relecture Maku dans cette version.

Le nuage initial échantillonne l'intérieur des triangles du terrain, en distinguant
les coordonnées de chaque tuile répétée. Un tampon d'identifiants suit exactement
l'ordre de peinture du moteur pour retenir les surfaces visibles. Les points
doivent être observés par au moins deux vues d'entraînement et rester à une
profondeur caméra inférieure à 200. Le ciel ne fournit pas de points.
Les vues de validation ne participent ni à la sélection finale des points ni à
leurs couleurs, qui moyennent uniquement les observations d'entraînement.

## Résultats et vérification

```powershell
ctest --test-dir cpp-offline/build -C Release --output-on-failure
```

Les tests de Maku vérifient le parcours contre les pistes ASE originales, les
instants XM et les cinq segments, les PNG, les poses, la focale, la reprojection
des points et leurs pistes réciproques. Ils comparent aussi les images d'un jeu
de 60 vues par passe aux instants correspondants d'un jeu de 120 vues par passe : elles doivent être
identiques malgré une densité, un split et des paramètres FPS/audio différents.
Ils vérifient H/4, l'orientation identique entre passes, les observations de points
communs et l'identité des images originales avec la seconde passe désactivée.
Le test dédié à la grille vérifie ses bornes contre l'ASE, l'agrandissement à
150 %, les stations et leurs 17 orientations, les poses verticales et les pistes
partagées avec les parcours. Il contrôle aussi l'identité des images originales
quand la grille est ajoutée, ainsi que l'indépendance de la grille vis-à-vis de
la densité temporelle, du décalage H/4 et des paramètres FPS. Les tests Saari et
Feta continuent de vérifier leurs exports.

Une comparaison avant/après du mode Maku classique, sur 40 images espacées d'une
seconde, son WAV et son manifeste, vérifie la conservation de ce rendu.

Résultats des deux premières itérations, en 1024 × 512 avec un FOV de 80° :

| Jeu | Parcours | PNG entraînement / validation | Points initiaux |
| --- | --- | ---: | ---: |
| `output-maku-gsplat` | Original | 270 / 30 | 29 202 |
| `output-maku-gsplat-h4` | Original + H/4 | 540 / 60 | 36 376 |

Les dossiers se trouvent dans `cpp-offline/`. Les deux tests CTest historiques
passaient avec la double capture. Les 42 fichiers de la comparaison Maku classique sont
identiques octet pour octet. Dans le nouveau jeu, les 300 PNG du parcours
original, leurs poses et leurs paramètres de caméra sont identiques à ceux du
premier jeu. Les deux hauteurs partagent 14 167 points observés.

Ces compteurs concernent le **nuage initial exporté**, pas le nombre de gaussiennes
après entraînement. Une augmentation du nombre de points ne prouve pas, à elle
seule, une meilleure reconstruction. Le gain sur les sommets et les trous doit
être évalué dans Postshot. Douze vues d'entraînement surélevées n'ont aucune
observation de point sous le seuil de profondeur 200 ; elles restent dans le
parcours complet demandé.

### Résultat de la troisième itération

Le jeu [`output-maku-gsplat-grid`](../cpp-offline/output-maku-gsplat-grid/) a été
généré et vérifié : **1 926 PNG**, dont **1 734 d'entraînement et 192 de validation**,
et **103 029 points initiaux**. La grille observe 93 008 de ces points ; **27 467**
sont partagés avec au moins un des deux parcours. Aucune vue d'entraînement de
la grille n'est dépourvue de point retenu. Cela mesure les observations du modèle
initial, sans préjuger du résultat du GSplat entraîné.

Les **600 PNG** des parcours nominal et surélevé sont identiques octet pour octet
au jeu `output-maku-gsplat-h4`, avec les mêmes poses et paramètres. Les ajouts
concernent les vues de grille et le modèle combiné. Les quatre tests CTest
passent ; les PNG, poses, projections et pistes réciproques du jeu complet ont
également été contrôlés. L'inspection visuelle de vues obliques et verticales
confirme que les sommets sont visibles et que le brouillard masque encore une
part importante des zones basses. La comparaison après entraînement reste à faire.

## Brouillard : ce qui est conservé et ce que le GSplat peut apprendre

Dans le [rendu C++](../cpp-offline/src/scenes/maku_scene.cpp), le brouillard dépend
de la **profondeur dans le repère de la caméra**, et non simplement de la distance
euclidienne au centre de celle-ci. Le facteur de fondu commence à croître à
81 unités et atteint presque son maximum vers 200 unités. Il pilote une rampe
de couleurs du terrain ; le calcul d'origine est conservé dans les parcours et la grille.

Le brouillard et la limite de sélection des points sont deux mécanismes distincts :
les PNG conservent l'effet complet du moteur, tandis que l'exporteur exclut les
points initiaux à une profondeur supérieure ou égale à 200. Une vue peut donc
contenir du brouillard sans fournir de points pour initialiser cette région.

**Le GSplat apprend les images produites, sans recevoir la loi de brouillard du
moteur.** Postshot utilise notamment des harmoniques sphériques pour représenter
les couleurs dépendant de la direction de vue. Cette capacité ne constitue pas,
à elle seule, une loi de variation avec la distance. La documentation ne garantit
pas une restitution exacte du brouillard de Maku.
[Source : configuration d'entraînement Jawset](https://www.jawset.com/docs/d/Postshot%2BUser%2BGuide/Interface/Training%2BConfiguration).

L'attente pour cette expérience est donc une reproduction plausible du voile près
des vues d'entraînement, avec une incertitude plus forte lorsqu'on s'en éloigne.
Par exemple, un rocher appris avec des couleurs très délavées peut rester trop
pâle quand on s'en approche. Des gaussiennes semi-transparentes peuvent aussi
contribuer au voile. Ce sont des comportements possibles, pas des défauts déjà
confirmés sur le nouveau GSplat.

La passe haute ajoute des observations depuis d'autres positions. Elle peut
améliorer la couverture, mais certaines vues deviennent presque blanches et
apportent peu de détails. Le décalage H/4 limite l'élévation par rapport au H/2
initialement envisagé, sans garantir à lui seul de résoudre ce problème.

Les poses connues évitent de devoir les estimer à partir d'images très embrumées.
Elles ne rendent pas le brouillard indépendant du point de vue et ne corrigent
pas les textures affines du moteur d'origine.

## Évaluation à effectuer dans Postshot

1. Comparer les itérations avec des réglages d'entraînement comparables et les
   mêmes points de vue de contrôle : parcours original, vues hautes et positions
   intermédiaires. Vérifier si les sommets et les zones manquantes sont mieux couverts.
2. Choisir un relief identifiable et avancer vers lui, puis reculer, en gardant
   approximativement la même direction. Observer si son contraste revient à
   l'approche et diminue à l'éloignement, sans apparition de nappes ou de ruptures.
3. Confronter ce comportement au rendu C++ aux mêmes poses lorsque ces captures
   sont disponibles. Les images réservées dans `validation/` constituent déjà des
   références qui n'ont pas participé à l'entraînement.
4. Séparer les problèmes de couverture, de brouillard et de textures affines avant
   de décider d'ajouter un nouveau parcours.

## Pistes ultérieures, non implémentées

| Piste | Principe et intérêt | Point à vérifier |
| --- | --- | --- |
| Couverture verticale élargie | Incliner certaines vues du parcours vers le haut, ou passer en 1024 × 768 à FOV horizontal constant | Le format actuel 2:1 donne seulement 45,5° de FOV vertical ; le 4:3 donnerait environ 64,4° |
| Caméras guidées par la couverture | Compter les observations des triangles avec le tampon de visibilité, puis cibler les surfaces peu vues | Vérifier aussi la diversité des positions et des angles, pas seulement le nombre d'images |
| Diagnostic sans brouillard | Refaire un jeu distinct aux mêmes poses pour isoler l'effet du brouillard sur les trous | Comparaison expérimentale ; le jeu retenu aujourd'hui conserve le brouillard |
| Parcours sur hémisphère | Ajouter des vues globales du secteur | Leur distance risque de faire disparaître le terrain dans le brouillard |

La grille implémentée fixe désormais l'emprise XY du **secteur à reconstruire**
à partir du parcours nominal. Cette bounding box n'est pas celle d'un décor fini
unique. Sa hauteur vient de la heightmap, comme pour H/4 ; le plan au-dessus du
sommet global ne garantit pas de bien observer les parties basses. Le pas de
grille et la marge verticale pourront être comparés avec les options dédiées.

La rotation à chaque station apporte des directions de vue supplémentaires ;
les déplacements entre stations apportent la parallaxe. Jawset recommande de
varier les positions, les hauteurs et les inclinaisons, avec environ 30 à 50 % de
recouvrement. Ici les poses connues simplifient le problème de suivi de caméra,
mais la diversité des positions reste utile à la reconstruction.
[Source : recommandations de capture Jawset](https://www.jawset.com/docs/d/Postshot%2BUser%2BGuide/Capturing%2BGuidelines).
