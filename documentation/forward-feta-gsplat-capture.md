# Feta : capture progressive pour Gaussian Splatting

État au 24 septembre 2026. L'export est implémenté dans la version offline C++11,
avec le même format PNG / poses COLMAP que Saari et Maku.

## Principe retenu

La caméra **commence loin et se rapproche progressivement du fœtus pendant sa
rotation**. La distance diminue sur tout le parcours, sans passes séparées à
rayon fixe. La caméra regarde le centre de la bounding box du mesh effectivement
rendu, qui sert aussi de centre à l'orbite.

Les vues couvrent le dessus, les côtés et le dessous. Le parcours effectue
**10,5 tours et trois cycles d'élévation**, pour revisiter les deux hémisphères
à différentes distances, y compris pendant le rapprochement final. Il approche
les pôles sans les traverser. Contrairement à Saari, aucun plan marin ne
restreint les positions.

Avec `u` variant de 0 à 1 entre la première et la dernière vue, la progression
radiale utilise `s = u² × (3 − 2u)`, puis `R = R_départ + (R_arrivée − R_départ) × s`.
La vitesse de rapprochement s'atténue aux deux extrémités. L'azimut augmente de
10,5 tours ; la composante verticale de la direction d'orbite vaut
`0,995 × cos(6πu)`. Les trois oscillations parcourent chaque fois le haut et le
bas, sans réserver les vues détaillées à un seul hémisphère.

Le temps de scène reste constant pendant toute la capture, à **0 seconde** par
défaut. Le mesh du fœtus est déjà statique dans ce rendu ; les centres des
**300 particules** sont évalués au même instant et restent aux mêmes positions
pour chaque caméra. Changer `--gsplat-time` choisit
une autre orientation figée du nuage, sans réintroduire une animation pendant
l'export. La cadence `--fps` n'a pas d'effet sur cet état.

Les deux traitements de rémanence du rendu sont supprimés pour la capture :
feedback et moyenne temporelle entre images, cette dernière produisant le motion
blur. Les fondus scriptés sont également désactivés. Les PNG dépendent de la
caméra et du temps figé, sans dépendre de la vue rendue juste avant.

## Ce que contient réellement la scène

L'examen de [feta_scene.cpp](../cpp-offline/src/scenes/feta_scene.cpp) distingue :

| Élément | Rendu conservé | Initialisation du GSplat |
| --- | --- | --- |
| Fœtus (`fetus.igu`) | Mesh fixe avec environment mapping `babyenv.jpg` | Échantillons de surface réellement visibles |
| Particules (`flare1.jpg`) | Sprites additifs, centres figés | Un point candidat au centre de chaque particule |
| Sphère apparente (`kosmusp.jpg`) | Fond panoramique échantillonné selon la direction du regard | Aucun point de géométrie finie inventé pour ce fond |

Les particules sont déjà des **billboards alignés sur le plan de l'image** dans
le moteur : chaque sprite est un carré projeté autour de son centre. Elles
restent donc face à la nouvelle caméra, même depuis le dessous. Seuls leurs
centres sont figés ; leur orientation d'affichage suit le point de vue.

Leur taille native est de `20 / profondeur` pixels à la focale native. En capture,
elle est multipliée par le rapport des focales : la taille physique reste
constante, environ **0,10925 unité** de côté, quelle que soit la résolution ou le
FOV. Le nuage initial contient les centres, pas des coins de billboards qui
changeraient de position avec la caméra.

Le rayon initial englobe le fœtus et les particules, avec leur demi-diagonale de
sprite. Le rayon final est calculé à partir du **fœtus seul**, pour augmenter sa
taille dans l'image. Les deux calculs tiennent compte du FOV horizontal **et
vertical**. Le mesh entier reste cadré sur le parcours automatique, tandis que
des particules peuvent sortir du cadre dans les vues rapprochées.

Le centre utilisé est environ `(0,00054 ; −0,15139 ; −0,01538)`, le rayon du
fœtus autour de ce centre **3,034 unités**, et celui de l'ensemble **8,041 unités**.
Avec les réglages par défaut, la caméra passe de **15,852 à 5,981 unités** de ce
centre. Le calcul utilise les 825 sommets du mesh, en excluant les coordonnées
de caméra et d'animation également présentes dans le fichier IGU. Le fond
panoramique n'a pas de rayon fini à inclure. La limite de profondeur native est
levée dans ce mode pour permettre des orbites plus éloignées.

## Produire et importer un jeu

Depuis la racine du dépôt, après compilation :

```powershell
cpp-offline/build/Release/forward-export.exe --sequence feta-gsplat --output cpp-offline/output-feta-gsplat-progressive
```

Le dossier de destination doit être nouveau. Les valeurs par défaut donnent
**300 PNG en 1024 × 768, FOV horizontal 80°**, dont **270 vues d'entraînement**
et **30 vues de validation**. `--frames` modifie le nombre total de prises.
`--gsplat-validation-every 0` affecte toutes les vues à l'entraînement.

Importer dans Postshot **`images/` avec `sparse/cameras.txt`, `sparse/images.txt`
et `sparse/points3D.txt`**. Garder `validation/` hors de l'entraînement ; ne pas
importer toute la racine du jeu. Comme pour Saari et Maku, les poses proviennent
du moteur et le nuage sert à initialiser la reconstruction.

Les fichiers d'accompagnement permettent de rejouer et d'inspecter la capture :

- `camera_path.csv` : positions et cibles natives, FOV horizontal et groupe ;
- `manifest.csv` : association entre PNG, pose, instant figé et sous-ensemble ;
- `particles.csv` : les 300 centres natifs, leur taille et leur ID de point candidat ;
- `capture.json` : état `complete`, centre, rayons de départ/arrivée/minimum/maximum,
  type de parcours, enveloppes, comptages et révision source ;
- `IMPORT.txt` : rappel des modalités d'import.

Le repère COLMAP inverse l'axe X natif et conserve Z vertical ; les poses sont
des transformations monde-vers-caméra. Les points sont filtrés par la visibilité
du rendu final et doivent avoir au moins deux observations d'entraînement. Les
pixels noirs du sprite ne masquent pas les points du fœtus. Un centre inscrit
dans `particles.csv` peut être absent de `points3D.txt` s'il manque d'observations.

Pour rejouer le parcours, utiliser `--gsplat-camera-path` avec le CSV exporté,
la même résolution et le même `--gsplat-time`. Le CSV impose les FOV de chaque
vue et remplace les options de génération de parcours. Ses groupes autorisés
sont `progressive`, `sphere` et `custom`. Les rayons variables sont acceptés,
ainsi que les anciens CSV à rayon constant autour de l'origine et les regards
exactement verticaux. Les caméras doivent rester hors de l'enveloppe du fœtus
avec une marge de 0,5 unité ; elles peuvent entrer dans l'enveloppe des particules.

Pour le parcours généré, `--gsplat-radius 0` calcule le rayon de départ et
`--gsplat-end-radius 0` le rayon final. Des valeurs positives permettent de les
fixer : le départ doit englober tous les objets et l'arrivée doit englober le
fœtus, avec 0,5 unité de marge. Le rayon final ne doit pas dépasser le rayon de
départ. Deux valeurs égales donnent une orbite à distance constante. Les options
sont détaillées dans le [README](../cpp-offline/README.md).

## Vérifications et prochaine évaluation

Le nouveau jeu progressif a été généré dans
[`cpp-offline/output-feta-gsplat-progressive/`](../cpp-offline/output-feta-gsplat-progressive/).
Il contient **300 PNG**, répartis en 270 vues d'entraînement et 30 de validation,
avec **6 885 points initiaux : 6 585 sur le fœtus et les 300 centres de particules**.
L'intégrité des images, les poses et les observations COLMAP ont été contrôlées
sur tout le jeu. Les 825 sommets du fœtus restent dans le cadre des 300 caméras.
Des vues larges, proches et polaires ont été inspectées visuellement.

Le premier jeu à rayon constant a été conservé dans
[`cpp-offline/output-feta-gsplat/`](../cpp-offline/output-feta-gsplat/).
Les **300 PNG** ont passé le contrôle d'intégrité ; le modèle contient
**6 878 points**, dont **6 578 échantillons du fœtus et les 300 centres de
particules**. Les poses et observations ont été contrôlées sur tout le jeu.
L'inspection visuelle a couvert des vues du dessus, de côté et du dessous.

Les tests d'intégration vérifient les PNG, la concordance des projections natives
et COLMAP, les liens réciproques images/points, les centres des particules et la
couverture des deux hémisphères. Pour le parcours progressif, ils vérifient la
décroissance continue du rayon, les huit octants dans chaque tiers du parcours,
le cadrage des 825 sommets et les bornes issues du mesh. Rejouer le parcours à
l'envers produit exactement
les mêmes images aux mêmes poses ; répéter une caméra donne aussi des PNG
identiques. Les essais incluent un autre instant figé, les deux pôles exacts et
des FOV différents. Les tests Saari et Maku continuent de passer.

Pour le rendu Feta classique, une comparaison avant/après couvre **35 images à
une seconde d'intervalle**, le WAV et le manifeste : les 37 fichiers sont
identiques octet pour octet. Les changements de capture préservent donc ce jeu
de référence du rendu natif.

La qualité du GSplat Feta reste à évaluer dans Postshot. Les observations locales
confirment que le fond dépend de la direction et que les sprites sont additifs ;
ce ne sont pas des surfaces opaques fixes ordinaires. L'environment mapping du
fœtus varie aussi avec la caméra. Il faudra observer la stabilité du fond et des
particules lors d'une navigation entre les prises, en plus de la silhouette du
fœtus. Le gel temporel et les poses connues éliminent deux sources d'ambiguïté,
sans garantir à eux seuls la restitution de ces effets.

Les enseignements précédents sont conservés dans la
[synthèse Saari et Maku](forward-maku-gsplat-capture.md) : adapter les positions
aux surfaces à observer, séparer les animations de la capture, conserver des
vues de validation et distinguer une couverture insuffisante des effets liés au
point de vue. Feta permet ici de tester une couverture intégrale sans les
contraintes de mer de Saari ni le parcours de terrain embrumé de Maku.
