# Feta : capture sphérique et halo pour Gaussian Splatting

État au 8 octobre 2026. Export dans la version offline C++11, au format PNG / COLMAP pour Postshot.

## Parcours retenu

La caméra reste désormais à **distance constante du centre du fœtus**, sur une
sphère. Ce choix remplace le rapprochement progressif par défaut, à la suite du
retour utilisateur indiquant de meilleurs résultats avec un rayon constant.
Le gain n'a pas été mesuré ici par un nouvel entraînement comparatif.

Le centre vient de la bounding box des 825 sommets effectivement rendus, en
excluant les coordonnées de caméra et d'animation du fichier IGU. Il vaut environ
`(0,00054 ; −0,15139 ; −0,01538)`. Le rayon automatique englobe le fœtus et les
particules avec une marge et tient compte des FOV horizontal et vertical :
**15,852 unités** avec les paramètres par défaut.

Le parcours conserve 10,5 tours et trois cycles d'élévation. Il couvre le dessus,
les côtés et le dessous sans traverser les pôles exacts. Le rayon ne varie plus
avec l'indice de vue. Les très petits écarts enregistrés dans le manifeste
proviennent des positions stockées en flottants. Aucun plan marin ne restreint
les positions.

Le temps local est figé à **0 seconde** par défaut. Le mesh est statique ; la
rotation des 300 centres de particules est évaluée au même instant pour toutes
les vues. `--gsplat-time` choisit un autre état figé ; `--fps` ne le fait pas avancer.

## Origine du halo jaune et adaptation de capture

Le code Java reconstruit précise le mécanisme :
[`FetaScene.java`](../java-desktop/src/main/java/FetaScene.java), méthodes
`KAmAJAk`, `KamAJAk` et `kAMAJAk`. Le modèle n'est pas dupliqué en géométrie 3D
pour cet effet. Ses pixels sont marqués par le bit de signe de la texture
d'environnement ; ce masque alimente un **feedback en espace écran**.

À chaque image de la lecture originale :

- les pixels marqués du fœtus imposent la valeur 255 dans le masque ;
- ailleurs, le masque précédent est agrandi d'un facteur 1,1 autour du centre
  d'image, puis son indice est divisé par deux ;
- la palette `(min(255, 2i), min(255, 3i), i)` donne sa couleur au halo,
  ajoutée avec saturation au rendu ; les pixels marqués du fœtus restent intacts.

L'ancien mode de capture supprimait tout le feedback pour éviter les traces de
la caméra précédente. Il supprimait ainsi le halo.

Le nouveau mode calcule **l'état stationnaire de ce même feedback pour chaque
caméra indépendamment**, avec la palette et l'échantillonnage fixe 16.16 natifs.
Il remonte les échantillons successifs jusqu'au masque du fœtus : les indices
possibles sont 127, 63, 31, 15, 7, 3 et 1. Après sept étapes, aucun indice non nul
supplémentaire ne subsiste. Cela reproduit le halo d'une caméra immobilisée sans
conserver de tampon provenant d'une autre vue.

L'algorithme accepte la résolution exportée, y compris des dimensions non
puissances de deux. La moyenne temporelle produisant le motion blur et les fondus
scriptés restent désactivés. La lecture classique conserve son chemin de rendu.

Le halo reste un effet lié au point de vue, présent dans les PNG. Il ne reçoit
**ni coque 3D artificielle, ni points initiaux supplémentaires, ni observations
opaques inventées**. Sa qualité après reconstruction doit être vérifiée dans
Postshot, surtout entre les vues ou lors d'un changement de distance.

## Éléments conservés

| Élément | Apparence | Initialisation |
| --- | --- | --- |
| Fœtus (`fetus.igu`) | Environment mapping `babyenv.jpg` | Échantillons des surfaces visibles |
| Halo | Masque agrandi, palette jaune, composition additive stationnaire | Aucun point ajouté |
| Particules (`flare1.jpg`) | Centres figés, billboards additifs face à chaque caméra | Centres visibles |
| Fond (`kosmusp.jpg`) | Panorama dépendant de la direction du regard | Aucun faux fond de rayon fini |

La taille des sprites suit le rapport des focales et la profondeur : leur taille
physique native reste environ 0,10925 unité. La limite de profondeur native est
levée en capture pour permettre les orbites éloignées. L'environment mapping et
la composition additive conservent leurs variations selon le point de vue.

## Produire et importer un jeu

Depuis la racine du dépôt, après compilation :

```powershell
cmake --build cpp-offline/build --config Release
cpp-offline/build/Release/forward-export.exe --sequence feta-gsplat --output cpp-offline/output-feta-gsplat-sphere-halo
```

Choisir un nouveau dossier si celui-ci existe déjà. Les paramètres par défaut
donnent **300 PNG de 1024 × 768, FOV horizontal 80°**, répartis en **270 vues
d'entraînement et 30 vues de validation**.

Importer ensemble **`images/`, `sparse/cameras.txt`, `sparse/images.txt` et
`sparse/points3D.txt`** dans Postshot. Garder `validation/` hors de l'apprentissage ;
ne pas importer toute la racine du jeu.

| Option | Comportement |
| --- | --- |
| `--gsplat-radius 0` | Rayon automatique englobant le mesh et les particules |
| `--gsplat-radius R` | Rayon de départ explicite, avec marge de 0,5 unité autour de l'enveloppe complète |
| `--gsplat-end-radius 0` | Rayon constant, identique au départ ; nouveau défaut |
| `--gsplat-end-radius R` | Valeur positive inférieure au départ : rapprochement progressif explicite ; elle doit englober le fœtus avec marge de 0,5 unité |
| `--gsplat-halo on` | Halo stationnaire activé ; défaut |
| `--gsplat-halo off` | Comparaison sans halo, avec les mêmes caméras et la même géométrie |
| `--gsplat-camera-path CSV` | Rejouer les positions, cibles et FOV, y compris un ancien parcours progressif |
| `--gsplat-validation-every 0` | Affecter toutes les vues à l'entraînement |

Les options `--frames`, `--width`, `--height`, `--gsplat-fov` et `--gsplat-time`
restent disponibles. Deux rayons explicites égaux donnent également une sphère.
Un CSV n'est pas automatiquement reprojeté sur une sphère : il remplace le
parcours généré. Rejouer avec la même résolution, le même instant et le même
réglage de halo pour retrouver les PNG.

Les fichiers `camera_path.csv`, `manifest.csv`, `particles.csv`, `capture.json`
et `IMPORT.txt` conservent les paramètres et les associations image/caméra.
`capture.json` indique `path_kind: sphere` pour le nouveau parcours automatique
et `halo: stationary_native_feedback` ou `off`.

Le repère COLMAP inverse X natif, conserve Z vertical et décrit les poses
monde-vers-caméra. Les points doivent avoir deux observations d'entraînement ;
les couleurs et la sélection ne dépendent pas des vues réservées. Le halo ne
modifie pas les identifiants de surfaces ni les pistes d'observation. Les centres
de particules sans observations suffisantes restent dans `particles.csv`, mais
pas nécessairement dans `points3D.txt`.

## Résultats et contrôles du 8 octobre

Le nouveau jeu est dans
[`cpp-offline/output-feta-gsplat-sphere-halo/`](../cpp-offline/output-feta-gsplat-sphere-halo/).
Il contient **300 images et 6 884 points initiaux : 6 584 sur le fœtus et les
300 centres de particules**. Les rayons mesurés vont de 15,85211835 à 15,85211962
unités. L'intégrité des 300 PNG et le modèle COLMAP complet ont été contrôlés ;
l'erreur maximale de reprojection interne mesurée est de 2,54 × 10⁻¹³ pixel.
La projection est aussi comparée indépendamment à la projection native.

Le test C++ `feta-static-halo` compare trois poses figées au rendu classique
stabilisé sur 32 images. L'écart maximal autorisé et vérifié est de 1/255 par
canal, dû à l'arrondi de la moyenne temporelle native supprimée en capture.
Il vérifie également l'addition, la présence du halo hors du mesh, l'absence de
teinte ajoutée au fœtus, l'identité des surfaces et les dimensions non natives.

Le test d'intégration Feta vérifie le rayon constant, les deux hémisphères, le
cadrage du mesh, les poses et pistes réciproques, les pôles exacts, le replay
inversé et les vues répétées. La comparaison halo activé/désactivé confirme des
images différentes avec des géométries, caméras et observations identiques.
Le rapprochement explicite et les anciens CSV restent couverts.

Les tests Saari, Maku et grille Maku passent également. Une nouvelle comparaison
avant/après du mode `feta` classique porte sur **35 images espacées d'une seconde,
le WAV et le manifeste : les 37 fichiers sont identiques octet pour octet**.

Les vues de repérage avec et sans halo sont conservées dans
`cpp-offline/output-feta-sphere-halo-scout/` et
`cpp-offline/output-feta-sphere-no-halo-scout/`. Des vues latérales et polaires
ont été inspectées. Aucun nouvel entraînement Postshot n'a été effectué pour
cette révision ; la stabilité spatiale du halo reste à évaluer dans le splat.

## Variante à 1024 pixels de hauteur

Un second jeu a été produit en **1366 × 1024**, en rejouant exactement le CSV du
jeu sphérique avec halo. Le rayon, le temps figé, les orientations et le FOV
horizontal sont identiques. Le ratio d'image est arrondi à un nombre entier de
pixels, très proche du 4:3 initial. La focale exportée suit la nouvelle largeur.

```powershell
cpp-offline/build/Release/forward-export.exe --sequence feta-gsplat --width 1366 --height 1024 --gsplat-camera-path cpp-offline/output-feta-gsplat-sphere-halo/camera_path.csv --output cpp-offline/output-feta-gsplat-sphere-halo-h1024
```

Le [jeu haute résolution](../cpp-offline/output-feta-gsplat-sphere-halo-h1024/)
contient 270 PNG d'entraînement, 30 de validation et 6 880 points initiaux, dont
les 300 centres de particules. La sélection des points dépend de la visibilité
au pixel près ; elle peut donc légèrement varier avec la résolution.

La hauteur projetée médiane du mesh passe de **186,1 à 248,2 pixels**, soit un
gain d'environ 33 % en définition linéaire. Les 825 sommets restent cadrés dans
les 300 vues. Les PNG, les poses, les observations réciproques et la reprojection
native/COLMAP ont été vérifiés sur tout le jeu. `camera_path.csv` est identique
octet pour octet à celui du jeu précédent. Le dossier occupe environ 176 Mo et
contient un `validation-report.json`.

Les images sont rendues directement à cette résolution. Le surcroît de pixels
améliore l'échantillonnage du mesh, du halo et des particules ; les textures
d'origine gardent leur définition. L'exporteur conserve son défaut 1024 × 768 ;
les options ci-dessus sélectionnent cette variante. Aucun entraînement Postshot
n'a été lancé pour cette comparaison.

## Échantillonnage octaédrique et décalage de cible

Le mode `--gsplat-sampling octahedral` reprend le `octDecode` GLSL fourni dans
la demande : centres d'une grille carrée dans `[-1,1]²`, pliage de l'hémisphère
inférieur, puis normalisation. Les références fournies sont le
[message de m4xc](https://bsky.app/profile/m4xc.bsky.social/post/3lbfsnxviz225)
et [JCGT 3(2), article 1](https://jcgt.org/published/0003/02/01/).
L'implémentation suit le code fourni ; elle ne prétend pas répartir exactement
la même aire sphérique par cellule.

Sans `--frames`, ce mode génère **324 vues, soit 18 × 18**. Un nombre explicite
doit être un carré parfait ; 300 est refusé plutôt que de tronquer la grille.
Le rayon doit rester constant. Le mode `orbit` et ses 300 vues restent le défaut
général de l'exporteur.

La variante retenue pour cet essai garde les origines octaédriques et décale
seulement la cible. `--gsplat-target-offset 0.15` répartit les cibles dans un
disque de rayon **0,15 unité**, centré sur le centre du mesh et perpendiculaire
à la direction caméra-centre. Une progression à angle d'or, avec
`r = 0,15 × sqrt((i + 0,5) / N)`, répartit les échantillons en aire dans ce disque.
Le repère du disque suit chaque caméra. Cela varie légèrement le cadrage ;
la parallaxe provient des positions différentes sur la sphère.

```powershell
cpp-offline/build/Release/forward-export.exe --sequence feta-gsplat --gsplat-sampling octahedral --gsplat-target-offset 0.15 --gsplat-radius 15.852119 --width 1366 --height 1024 --output cpp-offline/output-feta-gsplat-octahedral-target015-h1024
```

Le [jeu avec cibles décalées](../cpp-offline/output-feta-gsplat-octahedral-target015-h1024/)
contient **292 PNG d'entraînement et 32 de validation en 1366 × 1024**, avec le
halo et **6 881 points initiaux** : 6 581 sur le fœtus et 300 particules.
Le [témoin centré](../cpp-offline/output-feta-gsplat-octahedral-h1024/)
utilise les mêmes paramètres avec `--gsplat-target-offset 0`.
Le nombre de points retenus est ici identique ; leurs observations peuvent varier
avec le cadrage et la visibilité au pixel près.

Les positions et FOV exportés des deux jeux sont exactement identiques. Leur
distance au centre reste **15,852119 unités**, aux arrondis flottants près.
La variation maximale de visée mesurée est **0,5417°**, soit un déplacement du
centre du mesh d'au plus **7,70 pixels**. Les **825 sommets** restent cadrés dans
les 324 vues. Les PNG, les poses, les pistes réciproques et la reprojection
native/COLMAP ont été contrôlés sur tout le jeu. Voir le
[rapport de validation](../cpp-offline/output-feta-gsplat-octahedral-target015-h1024/validation-report.json)
et la [répartition des cibles](../cpp-offline/output-feta-octahedral-comparison/target-offsets.png).

La régularité des origines a été mesurée sur les CSV, indépendamment des images :

| Mesure, toutes les vues | Ancienne orbite, 300 vues | Octaédrique, 324 vues |
| --- | ---: | ---: |
| Plus petit angle entre deux caméras | 0,42° | 6,73° |
| Écart-type / moyenne de l'angle au voisin le plus proche | 0,722 | 0,201 |
| Plus grande distance angulaire au point de vue le plus proche, estimée | 24,65° | 8,79° |

La dernière mesure utilise 20 000 directions de contrôle Fibonacci ; il s'agit
d'une estimation de couverture, pas du rayon de couverture exact. Un contrôle
analytique de l'ancienne orbite à **324 vues** donne encore 24,72° et un rapport
écart-type/moyenne de 0,709 : le gain ne vient donc pas seulement des 24 vues
supplémentaires. En ne conservant que les **292 vues d'entraînement**, l'écart
maximal estimé du nouveau jeu est 14,58°. Le décalage de cible conserve toutes
ces mesures puisqu'il ne modifie aucune origine.
Le [graphique comparatif](../cpp-offline/output-feta-octahedral-comparison/sampling-comparison.png),
les mesures JSON et les scripts de contrôle sont conservés dans
`cpp-offline/output-feta-octahedral-comparison/`.

Un premier essai de décalage des origines reste disponible avec
`--gsplat-lateral-offset 0.15`, dans `output-feta-gsplat-octahedral-offset015-h1024/`.
Il applique un déplacement tangent alterné, puis renormalise sur la même sphère.
La variante avec cibles décalées laisse cette option à zéro. Les deux offsets
sont déterministes, exprimés en unités monde, limités à `[0,1]` et nuls par défaut.

Les CSV enregistrent les cibles effectives dans `tx,ty,tz`. Les groupes acceptés
incluent désormais `octahedral`, `octahedral_target` et `octahedral_offset`.
Le replay utilise les poses enregistrées sans appliquer à nouveau les offsets.
`capture.json` conserve le mode, le côté de grille, les offsets demandés et
appliqués, ainsi que `target_offset_pattern: golden_angle_disk_camera_plane`.
En replay, les offsets appliqués valent zéro car aucune perturbation supplémentaire
n'est ajoutée au CSV.

Les **six CTests passent**, dont le nouveau `feta-octahedral-export` : grille
inverse, rayon constant, origines inchangées lors du décalage de cible, disque,
pôles exacts, entrées invalides et replay inversé avec PNG identiques.
Aucun entraînement Postshot n'a été lancé pour ces jeux ; l'intérêt du décalage
de cible pour la reconstruction reste à comparer au témoin centré.

## Dernier essai : particules agrandies et nuage élargi

Le [jeu avec particules ×2 et nuage ×2](../cpp-offline/output-feta-gsplat-octahedral-target015-particles2-cloud2-h1024/)
reprend exactement les 324 caméras du jeu octaédrique avec cibles décalées.
Les deux réglages sont propres à la capture C++ :

- `--gsplat-particle-size-scale 2` double le côté des sprites en unités monde,
  de 0,10925 à **0,21850 unité**, soit une surface quadruplée à profondeur égale ;
- `--gsplat-particle-cloud-scale 2` multiplie les centres par deux autour de
  l'origine native, avant la rotation figée. Le cube initial d'environ `[-5,5]³`
  devient `[-10,10]³`, avec les **mêmes 300 particules**.

Les positions sont communes à toutes les vues. La construction des billboards,
les tests de visibilité, les points COLMAP, `particles.csv` et les bornes de
géométrie utilisent tous ces nouveaux paramètres. Les options acceptent `[0,1 ; 10]`
et valent 1 par défaut. Elles s'appliquent aussi lors du replay CSV : le CSV décrit
les caméras, pas les modifications de la scène.

```powershell
cpp-offline/build/Release/forward-export.exe --sequence feta-gsplat --gsplat-particle-size-scale 2 --gsplat-particle-cloud-scale 2 --width 1366 --height 1024 --gsplat-camera-path cpp-offline/output-feta-gsplat-octahedral-target015-h1024/camera_path.csv --output cpp-offline/output-feta-gsplat-octahedral-target015-particles2-cloud2-h1024
```

Le cadrage automatique continue à utiliser l'enveloppe native, avant mise à
l'échelle des particules. Cela évite de reculer la caméra quand on élargit le
nuage. Les particules peuvent donc déborder du cadre ou entourer certaines
caméras ; le clipping et la visibilité restent évalués normalement.
`capture.json` distingue l'enveloppe réelle (`enclosing_radius`, environ 16,005)
de l'enveloppe native utilisée pour le cadrage (`camera_framing_enclosing_radius`,
environ 8,041), et enregistre les deux facteurs d'échelle.

Le jeu contient **292 images d'entraînement, 32 de validation, en 1366 × 1024**,
avec halo, et **6 881 points initiaux**, dont les 300 centres de particules.
`camera_path.csv` est identique octet pour octet au jeu précédent : rayon
15,852119, cibles décalées et FOV sont conservés.

Le contrôle des observations réellement visibles dans le modèle COLMAP trouve
des particules dans **les douze cases d'une grille 4 × 3 et dans les quatre
bandes périphériques de 10 % sur chacune des 324 images**. L'étendue médiane
des centres visibles atteint **97,0 % de la largeur et 98,7 % de la hauteur**.
Il reste au moins 204 centres observés par image, avec une médiane de 224.
Il s'agit d'un nuage discret réparti sur le cadre, pas d'un remplissage opaque
de chaque pixel. Voir le [comparatif visuel](../cpp-offline/output-feta-particles-review/particles-comparison.png)
et le [rapport complet](../cpp-offline/output-feta-gsplat-octahedral-target015-particles2-cloud2-h1024/validation-report.json).

Les six CTests passent. Les nouvelles vérifications couvrent les échelles de
taille et de position, la stabilité des caméras automatiques, les points du mesh,
la projection native/COLMAP et le replay inversé avec les mêmes échelles.
Les 324 PNG et leurs modèles sont contrôlés ; la lecture normale conserve les
**37 fichiers identiques** de la référence (35 images, WAV et manifeste).
Douze vues rejouées avec les échelles par défaut restent également identiques
aux PNG de l'essai précédent. Aucun entraînement Postshot n'a été lancé.

## Jeux antérieurs conservés

- `cpp-offline/output-feta-gsplat/` : première sphère centrée sur l'origine,
  sans halo, 300 images et 6 878 points.
- `cpp-offline/output-feta-gsplat-progressive/` : approche de 15,852 à 5,981 unités,
  sans halo, 300 images et 6 885 points.

Ces jeux documentent les essais précédents. La sphère actuelle utilise le centre
mesuré du mesh et n'est donc pas exactement le premier parcours. Pour isoler
l'effet du halo, comparer `on` et `off` avec un même `camera_path.csv`. Pour isoler
l'effet de la trajectoire, garder le même halo, l'état figé et des vues de
validation communes. Voir aussi la [synthèse Saari/Maku](forward-maku-gsplat-capture.md).
