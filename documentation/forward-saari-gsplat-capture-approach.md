# Saari : approche de capture pour Gaussian Splatting

Date : 19 septembre 2026.

Statut : proposition de travail, sans implémentation. Les paramètres ci-dessous sont des hypothèses à évaluer, et non un parcours déjà optimisé ou validé.

Complément documentaire : formats d'import Postshot et COLMAP vérifiés dans leurs documentations officielles le 19 septembre 2026. Aucun import du futur jeu Saari n'a encore été testé.

## Intention et point de départ

Une première reconstruction Gaussian Splatting de Saari a été calculée dans Postshot à partir d'une petite série d'images produites par l'exporteur C++ offline, en suivant uniquement la caméra de la démo. Les captures [vue d'ensemble](../postshot/2026-09-19_230622.png) et [vue rapprochée](../postshot/2026-09-19_230738.png) montrent une masse reconnaissable de l'île, entourée de nappes et d'étirements visibles depuis des points de vue extérieurs au parcours.

Ces images témoignent du résultat exploratoire ; elles ne permettent pas, seules, de mesurer sa fidélité sur la trajectoire d'apprentissage ni de déterminer la cause de chaque artefact.

La prochaine expérience envisagée consiste à produire un jeu d'images spécifiquement destiné au gsplat : **couvrir l'île entière et les objets autour, avec une caméra se déplaçant sur une hémisphère englobante et klunssi figé**. La caméra doit toujours rester au-dessus du plan de la mer. Le résultat recherché est une représentation explorable de l'apparence de Saari à un instant choisi.

La couverture visée inclut les côtes, les versants, le sommet, la figure « meditate », klunssi et leur contexte visuel sur la mer. **Un focus spécifique sur meditate complète la couverture générale de l'île.** La capture concerne les surfaces observables depuis des caméras au-dessus de l'eau. Le ciel et l'horizon participent aux images, mais la reconstruction d'une mer illimitée ne constitue pas un objectif mesurable pour cette première expérience.

## Séparer le temps de la scène du parcours de capture

Le principe central est de fixer un temps de scène `t₀`, puis de faire varier uniquement la caméra entre les prises de vue.

Dans le rendu actuel, le temps de Saari pilote à la fois la caméra originale et l'animation de klunssi. Le gel devra conserver ensemble :

- la position de klunssi sur sa trajectoire ;
- sa rotation sur les trois axes ;
- les mêmes transformations pour son reflet.

Les événements visuels transitoires du script, notamment les effets `suh` / `suh0`, devront être désactivés pour ce jeu de capture. La musique et la durée de lecture de la démo ne détermineraient plus le nombre de vues. Revenir à une même caméra et au même `t₀` devrait produire exactement la même image.

Un instant proche de **30 secondes de temps local de Saari** est un candidat de départ : la piste de position place alors klunssi près de l'île. Ce choix devra être confirmé sur des vues de repérage afin d'éviter une intersection avec le terrain, une occultation excessive ou une pose peu lisible. Il ne s'agit pas de 30 secondes depuis le début de la démo complète.

Figer les objets conserve les variations d'apparence liées au point de vue : les reflets et l'environment mapping doivent continuer à être calculés depuis chaque caméra.

## Déplacer la caméra sur une hémisphère englobante

Le domaine de capture retenu est la **surface d'une hémisphère supérieure qui englobe l'île et les objets dans leur pose figée**. Son centre `C = (cx, cy, z_mer)` est placé sur le plan de la mer, sous le centre horizontal du volume à couvrir. Son rayon `R` doit contenir le terrain émergé, meditate et klunssi, avec une marge extérieure. Les positions virtuelles des reflets sous l'eau ne servent pas à définir ce volume.

Une position de caméra `P` serait définie par un azimut `θ` et une élévation `φ` au-dessus de l'horizon :

- `P.x = cx + R × cos(φ) × cos(θ)` ;
- `P.y = cy + R × cos(φ) × sin(θ)` ;
- `P.z = z_mer + R × sin(φ)`.

Le rayon sphérique reste constant : à mesure que la caméra monte, le rayon horizontal de son orbite diminue. Une spirale sur cette surface ou une succession de cercles de latitude reliés sur la sphère permettraient d'organiser les prises de vue. Le nombre de tours dépendra du recouvrement obtenu entre les passages.

**La contrainte `P.z ≥ z_mer + ε`, avec une marge `ε > 0`, s'applique à toutes les positions du parcours.** Elle exclut l'équateur exact, les vues sous-marines et les passages trop proches du plan de l'eau. Cela revient à imposer `φ ≥ arcsin(ε / R)`, avec `0 < ε < R`. Cette limite devra être respectée aussi entre les prises de vue, lors des transitions et pendant la navigation de validation. Les transitions seront construites sur la surface autorisée ; une interpolation libre en coordonnées cartésiennes pourrait couper à travers la sphère et le relief.

Cette exclusion répond à la cohérence visuelle recherchée : le dessous de la mer ne constitue pas un domaine de capture pertinent pour cette scène. Les reflets seront observés depuis le dessus de l'eau, sans ajouter de caméras symétriques sous le plan marin.

La cible du regard peut se situer au-dessus du centre de l'hémisphère, dans le volume de l'île et des objets. Elle n'a pas besoin de coïncider avec `C`. Près du zénith, il faudra conserver une orientation stable : la construction actuelle à partir d'un axe vertical devient ambiguë pour un regard exactement vertical. Un premier parcours pourra s'arrêter légèrement avant le pôle, puis compléter cette zone si nécessaire après vérification de l'orientation.

| Partie du parcours | Contribution recherchée | Point à vérifier |
| --- | --- | --- |
| Bande basse de l'hémisphère | Côtes, silhouette des versants, klunssi et contexte des reflets | Respecter la marge au-dessus de l'eau et garder suffisamment de terrain dans l'image |
| Bande intermédiaire | Faces latérales de l'île et des objets, vues communes avec les autres passages | Garder des détails partagés entre les prises de vue |
| Calotte haute | Sommet, figure meditate, surfaces masquées depuis le bas | Stabiliser l'orientation près du zénith et éviter les vues presque identiques |
| Compléments locaux sur la même surface, si nécessaires | Faces encore mal couvertes de klunssi ou du sommet | Relier ces vues aux vues générales par des prises intermédiaires |

Le rayon doit dépendre de l'enveloppe de l'île **et de la pose figée des objets**. On pourra partir de la distance maximale à `C` des points de cette géométrie, augmentée d'une marge. Il faudra ensuite vérifier le cadrage : englober les objets dans une sphère ne garantit pas qu'ils tiennent dans chaque image. Un rayon excessif peut perdre les petits détails et accentuer le brouillard. Le centre, le rayon, la cible et les limites d'élévation restent à ajuster sur une planche de repérage.

Un premier budget de **240 à 360 vues**, réparties sur la surface autorisée, permettrait une expérience comparable en ordre de grandeur à une petite séquence. C'est une hypothèse de travail. Une répartition uniforme en latitude et en azimut accumulerait trop de vues près du pôle. Une base plus régulière en surface consiste à répartir les bandes selon `sin(φ)` et à adapter le nombre de vues à leur circonférence, puis à ajuster cette répartition selon les détails visibles et leur recouvrement.

La sélection des vues devrait tenir compte de la translation, de l'orientation et des surfaces visibles. Des images presque identiques apportent peu d'information supplémentaire ; une rotation sur place ne fournit pas la diversité de positions recherchée. On densifierait les prises près des objets ou des changements d'occultation, après inspection.

Les [recommandations de capture de Jawset](https://www.jawset.com/docs/d/Postshot+User+Guide/Capturing+Guidelines) vont dans ce sens : scène statique, plusieurs anneaux à des hauteurs différentes, déplacement de la caméra et maintien du contexte autour des détails. Elles donnent un recouvrement indicatif de 30 à 50 %. Pour Saari, ce pourcentage serait un repère à confronter aux surfaces effectivement visibles, sans remplacer l'inspection du sommet et de meditate. Fournir des poses exactes ne supprime pas le besoin de points de vue complémentaires.

Le parcours serait décrit dans un fichier éditable contenant au minimum, pour chaque vue, la position de caméra et sa cible. Le centre de l'hémisphère, son rayon, le niveau de la mer, la marge `ε` et les limites d'élévation seraient conservés avec lui. Toute modification du parcours devrait rester sur cette surface et respecter la limite marine. Cela permettrait d'ajuster la capture sans dépendre de la piste originale et de rejouer exactement une expérience.

## Prévoir un focus sur meditate

La figure meditate, placée au-dessus du sommet, doit recevoir un ensemble de vues dédiées. Le but est de conserver sa silhouette, ses volumes et son rapport au sommet, tout en l'intégrant à la reconstruction de l'île.

Les positions de caméra resteraient sur la même hémisphère englobante, avec la même marge au-dessus de la mer et le même temps de scène figé. Le focus agirait d'abord sur la cible du regard et sur la densité des vues :

- orienter une partie des prises vers le centre de la figure plutôt que vers le centre général de l'île ;
- couvrir plusieurs azimuts et plusieurs élévations donnant des faces complémentaires, après vérification des occultations par le sommet ;
- densifier les vues là où la silhouette ou les surfaces visibles changent rapidement ;
- conserver des images montrant à la fois la figure et une portion identifiable du sommet pour relier ce groupe aux vues générales ;
- prévoir des transitions progressives du regard entre l'île et meditate, sans quitter la surface autorisée.

Un simple changement de cible ne garantit pas davantage de pixels sur la figure. La planche de repérage devra donc vérifier sa taille effective dans l'image. Si le niveau de détail est insuffisant, on pourra étudier un rendu à résolution réellement supérieure ou un groupe de vues avec un champ de vision plus étroit. Dans ce second cas, la projection serait constante à l'intérieur du groupe, enregistrée pour chaque image et compatible avec le pipeline retenu. Les vues de liaison garderaient suffisamment de contexte ; un recadrage ou un agrandissement après rendu ne créerait pas de détails supplémentaires.

Le budget initial de 240 à 360 vues serait d'abord réparti entre couverture générale et focus, puis augmenté si la figure exige davantage de prises. La répartition exacte serait décidée après le repérage, afin de ne pas laisser un versant de l'île sans couverture pour densifier le sommet.

Des vues de validation centrées sur meditate seraient réservées avant l'apprentissage. Elles permettraient d'examiner sa silhouette, sa séparation visuelle du terrain, la stabilité de ses volumes et les éventuels éléments flottants autour de la figure, indépendamment de la qualité globale de l'île.

## Préserver l'apparence tout en documentant ses limites

Saari emploie des conventions de rendu particulières : interpolation affine de certaines textures, tri en profondeur, réflexions additives masquées et brouillard dépendant de la profondeur. Le premier essai conserverait ces choix pour rester attaché à l'apparence de la démo.

Même avec klunssi immobile, ces propriétés peuvent compliquer la cohérence entre vues. En particulier, une texture interpolée en affine peut se déplacer visuellement sur un triangle quand la caméra bouge. Une meilleure couverture et le gel de l'animation ne garantissent donc pas une reconstruction sans nappes ni éléments flottants.

Si des problèmes persistent, des variantes ciblées pourront servir au diagnostic, en ne changeant qu'un facteur à la fois. Une éventuelle capture avec matériaux ou réflexions simplifiés devra être identifiée comme une variante expérimentale, avec ses écarts documentés.

L'exporteur actuel impose à Saari une sortie native de **512 × 256**. Une capture à résolution supérieure serait une évolution ultérieure : elle demanderait de vérifier la projection et les calculs qui dépendent des dimensions. Agrandir les images après rendu n'ajouterait pas de détails géométriques ou de texture.

## Conserver les images et leurs caméras

Le moteur connaît les caméras qu'il utilise. **Le futur jeu de capture devrait fournir ces poses exactes à Postshot**, avec un nuage de points initial, au format COLMAP décrit ci-dessous. Le CSV du parcours resterait l'archive éditable des intentions de capture.

| Donnée | Utilité |
| --- | --- |
| Images sans perte, numérotées | Entrées d'apprentissage reproductibles ; PNG pour l'échange, TGA natif à conserver si utilisé |
| Position et orientation effectives de chaque caméra | Retrouver les vues exactes et permettre une utilisation ultérieure des poses connues |
| Projection, dimensions et conventions de coordonnées | Interpréter correctement focale, centre de projection, axes et sens des matrices |
| Temps local figé et état des effets | Reconstituer la scène statique choisie |
| Parcours, hémisphère et limite marine | Reproduire la couverture et vérifier le rayon ainsi que la marge au-dessus de l'eau |
| Groupe de capture : île, liaison ou focus meditate | Retrouver la répartition des vues et évaluer séparément la figure |
| Version du code, références des assets et réglages d'apprentissage | Relier le splat à ses sources et comparer les expériences |
| Répartition apprentissage / validation | Mesurer la restitution de vues absentes de l'entraînement |
| Points 3D initiaux, couleurs et observations associées | Initialiser la reconstruction dans le même repère que les caméras |

Il faudra exporter l'état réellement employé par le rendu. Par exemple, le parcours original construit sa base de caméra avant d'appliquer un plancher à sa position verticale : recalculer une orientation à partir d'une cible seule peut donc perdre une particularité du moteur.

## Exploiter l'import COLMAP de Postshot

### Format retenu

La page officielle [Importing Images](https://www.jawset.com/docs/d/Postshot+User+Guide/Importing+Images) confirme l'import conjoint d'images, de poses et de points 3D. Pour COLMAP, Jawset demande les trois fichiers `cameras`, `images` et `points3D`, en `.txt` ou `.bin`. Les images doivent correspondre aux poses ; les entrées sans correspondance sont ignorées. Avec cet import, Postshot peut passer directement à l'apprentissage sans refaire le suivi des caméras. Le CSV générique du parcours ne constitue donc pas, à lui seul, le jeu d'import retenu.

Le choix proposé est **COLMAP texte**, pour pouvoir inspecter et archiver les valeurs. Organisation envisagée pour le projet : un dossier `images/` contenant les PNG, un dossier `sparse/` contenant les trois fichiers texte, et les métadonnées de capture à côté. Produire ce format ne nécessite pas de faire estimer les caméras par COLMAP.

| Fichier | Contenu à produire |
| --- | --- |
| `cameras.txt` | Modèle de projection, dimensions et paramètres internes |
| `images.txt` | Pose de chaque vue, identifiant de caméra, nom d'image et observations 2D |
| `points3D.txt` | Points colorés et liens vers leurs observations |

Dans le [format COLMAP](https://colmap.github.io/format.html), une image possède une transformation **monde vers caméra** : `X_camera = M × X_world + t`. La rotation est stockée en quaternion Hamilton, dans l'ordre `qw, qx, qy, qz`. La translation vaut `t = -M × C_camera` ; elle n'est pas la position de caméra. Les axes caméra sont droite, bas, avant. Chaque image occupe deux lignes, dont la seconde décrit ses observations 2D. Les observations et les pistes des points doivent se référencer réciproquement ; les indices d'observation commencent à zéro.

### Projection et changement de repère de Saari

Le modèle `PINHOLE` est adapté à une projection sans distorsion optique, avec les paramètres `fx, fy, cx, cy`. Voir les [modèles de caméra COLMAP](https://colmap.github.io/cameras.html). D'après le code actuel de Saari, la projection utilise `fx = fy = (largeur / 2) / tan(1,4 / 2)`, `cx = largeur / 2` et `cy = hauteur / 2`. Pour 512 × 256, la focale est donc d'environ 303,934 pixels. Ces valeurs devront provenir de l'état effectif du moteur lors de chaque capture.

Les paramètres pourraient être partagés par toutes les images de même projection. Si le focus meditate emploie un champ plus étroit, il recevrait un autre identifiant de caméra. Une modification de résolution ou de cadrage devrait également être répercutée dans les paramètres exportés.

La [FAQ COLMAP](https://colmap.github.io/faq.html#using-calibration-from-opencv-kalibr-or-other-tools) place le centre du premier pixel à `(0,5 ; 0,5)`. Cela correspond aux échantillons du rasteriseur Saari consulté ; il ne faut pas ajouter automatiquement un décalage d'un demi-pixel.

**Déduction à partir du code de Saari : un changement de repère global est nécessaire.** Le moteur construit `right = worldUp × forward`, puis `up = forward × right`, et inverse le signe vertical lors de la projection. La matrice de lignes `[right ; -up ; forward]`, notée `A`, a donc un déterminant négatif hors cas dégénéré. Elle ne peut pas être convertie directement en quaternion de rotation.

Une conversion candidate consiste à appliquer `S = diag(-1, 1, 1)` à toutes les positions mondiales exportées, caméras et points compris. Avec `X_export = S × X_native` et `C_export = S × C_native`, on obtient `M = A × S` et `t = -M × C_export`. La matrice `M` a alors un déterminant positif et reproduit la projection native. Cette transformation conserve l'axe vertical et la limite du plan marin. Elle devra être documentée pour permettre le retour au repère original.

L'identité a été vérifiée numériquement sur trois poses et trois points de contrôle, avec un écart maximal inférieur à `3 × 10⁻¹⁴` en coordonnées caméra, en double précision. Ce contrôle algébrique ne constitue pas un test d'import Postshot ni une validation complète du rendu. Le futur export devra vérifier des reprojections sur les images produites, y compris près du sommet, avant tout apprentissage.

### Prévoir aussi les points d'initialisation

La documentation Jawset demande `points3D` dans l'import COLMAP ; elle n'établit pas ici qu'un fichier vide suffirait. Le plan doit donc inclure un nuage initial utilisable, au lieu de compter sur les seules poses.

Deux voies restent à comparer :

- **Échantillonner la géométrie connue.** Proposition propre au projet : créer des points sur le terrain, meditate et klunssi figé, avec une densité suffisante autour de la figure. Leurs couleurs et observations seraient rattachées aux vues où ils contribuent effectivement au rendu. Le tri et la composition particuliers de Saari demandent une vérification de visibilité ; une projection dans le cadre ne suffit pas à prouver qu'un point est visible. Le ciel et les reflets ne recevraient pas artificiellement les observations d'une surface opaque.
- **Trianguler depuis les poses connues.** La [FAQ COLMAP](https://colmap.github.io/faq.html#reconstruct-sparse-dense-model-from-known-camera-poses) décrit une chaîne d'extraction de caractéristiques, de correspondances puis de triangulation à partir de caméras fournies. Les identifiants d'images doivent correspondre à ceux de la base de données. Pour ce projet, il faudra contrôler les options de raffinement afin de conserver les paramètres exacts et vérifier qu'ils n'ont pas changé. Les textures affines et les reflets de Saari peuvent rendre certaines correspondances instables.

Le fichier de points initialement vide décrit dans cette procédure COLMAP est une entrée intermédiaire de triangulation ; il ne prouve pas qu'un import Postshot sans points fonctionne.

### Premier contrôle d'interopérabilité

Avant un jeu complet, un petit lot comprenant l'île, meditate et quelques points identifiables permettra de vérifier les noms d'images, les projections, les orientations et la présence du nuage dans le même repère. Les compteurs d'import et de poses devront correspondre au lot prévu. On contrôlera aussi l'absence d'inversion gauche-droite et le respect de la surface hémisphérique au-dessus de la mer.

Postshot permet de réexporter poses et points en COLMAP texte depuis l'Image Set : un aller-retour permettra de contrôler les données, en tenant compte de tout recentrage. Sa documentation précise aussi que le `.psht` référence les images externes sans les embarquer ; l'archive devra donc les conserver avec le projet. [Documentation Image Set](https://www.jawset.com/docs/d/Postshot+User+Guide/Interface/Scene+Tree/Image+Set)

L'avantage attendu est de supprimer l'incertitude de l'estimation des caméras pour cette scène synthétique. Il reste à mesurer le résultat : des poses exactes ne corrigent pas automatiquement les incohérences visuelles du rendu original.

## Évaluer ce que le nouveau parcours apporte

Trois jeux permettent de distinguer les deux changements principaux :

| Jeu | Caméra | État de klunssi | Rôle |
| --- | --- | --- | --- |
| A | Parcours original | Animation originale | Référence de l'expérience initiale |
| B | Parcours original | Pose figée à `t₀` | Observer l'effet du gel à vues comparables |
| C | Parcours sur l'hémisphère supérieure | Même pose figée à `t₀` | Observer le gain de couverture sur la même scène statique |

Les comparaisons doivent conserver autant que possible la résolution, le budget d'images et les réglages d'apprentissage. Si le jeu A initial n'est pas reproductible, il restera une référence qualitative, avec cette limite indiquée.

Pour comparer B et C, on conserverait également la même méthode d'export des poses et de construction du nuage initial. Une comparaison séparée, sur un même jeu d'images, pourrait examiner les poses estimées par Postshot et les poses exactes exportées. Elle devra consigner les différences de nuage initial, afin de ne pas attribuer leur effet à la seule précision des caméras.

Avant l'entraînement, on réserverait des vues de validation communes à B et C : points de vue proches du trajet original, petits déplacements latéraux, plusieurs azimuts et hauteurs autour de l'île. Ces vues respecteraient toutes la marge au-dessus du plan de la mer. On distinguerait les vues réservées sur l'hémisphère des vues intérieures, proches du parcours original, qui évaluent une autre liberté de déplacement. Leurs références seraient rendues avec le même temps figé. Comparer un splat statique à une image où klunssi a changé de pose mélangerait deux problèmes.

La validation combinerait des comparaisons d'images et une courte navigation continue. Elle rechercherait :

- des côtes, versants et objets lisibles depuis davantage de directions ;
- une figure meditate reconnaissable et stable depuis les vues réservées du sommet ;
- une diminution des traînées autour de klunssi ;
- moins de trous et d'éléments flottants près des zones couvertes ;
- une transition stable entre points de vue, sans apparition brutale de nappes ;
- le maintien des couleurs, des textures et de l'atmosphère de Saari.

Des métriques comme PSNR ou SSIM pourront compléter l'inspection sur les vues réservées. Elles ne remplaceront pas l'examen séparé du terrain, de klunssi et du sommet, ni l'observation en mouvement.

La première étape future serait une petite planche de repérage à plusieurs azimuts et élévations sur l'hémisphère, comprenant des cadrages généraux et des cadrages orientés vers meditate. Elle servirait à choisir la pose figée, corriger le cadrage et vérifier la visibilité ainsi que la taille de la figure dans les images avant de produire le jeu complet et de lancer un apprentissage. Le contrôle géométrique vérifierait la distance au centre, le respect de la marge au-dessus de la mer et l'absence d'intersection avec le relief ou les objets sur tout le parcours.

## Place dans la préservation et références

Ce splat constituerait une couche supplémentaire de préservation : une trace spatiale de l'apparence de Saari, associée à un instant, un ensemble de vues et un protocole de reconstruction. Le code, les assets, la version exécutable et les captures de référence resteraient les moyens de retrouver le comportement temporel de l'œuvre.

Sources du projet utilisées pour cette proposition :

- [Rendu C++ de Saari](../cpp-offline/src/scenes/saari_scene.cpp) : caméra, animation de klunssi, reflet et effets scriptés.
- [Scène ASE originale](../original/forward/asses/alku6.ase) : pistes et objets.
- [Documentation de l'exporteur offline](../cpp-offline/README.md) : fonctionnement et limites actuelles.
- [Méthodologie de parité du terrain](forward-saari-terrain-parity-methodology.md) : conventions de rendu et réflexions.
- [Investigation du ciel de Saari](forward-saari-sky-investigation.md) : traitement du fond et questions de fidélité.

Le principe général de représentation et de synthèse de vues est décrit dans [3D Gaussian Splatting for Real-Time Radiance Field Rendering, Kerbl et al., 2023](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/). Le parcours proposé ici est une hypothèse adaptée à Saari ; aucune optimisation de ce parcours ni mesure d'amélioration du splat n'a encore été effectuée.
