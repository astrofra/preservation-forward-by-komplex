# Forward Java Desktop

Base de reconstruction Java desktop de `forward`, derivee des sources decompilees de `reverse/cfr_single`.

## Ce qui a ete modernise

- suppression de la dependance `Applet` au profit d'un hote desktop AWT
- remplacement des anciens backends audio `IE3/IE4` / `sun.audio` par `Java Sound`
- correction des classes decompilees qui contenaient encore des artefacts `GOTO`
- desactivation du basculement intermediaire vers une fenetre ecran entier pour garder toute la demo dans la meme fenetre desktop
- recalage du scroll `phorward.gif` de `domina` et `uppol` sur une cadence virtuelle `50 Hz` pour eviter l'acceleration sur machines modernes
- recalage des composantes frame-dependantes de `mute95` sur une cadence virtuelle derivee du temps de scene pour limiter la surexposition de l'intro sans casser la fluidite du warp
- recalage des animations frame-dependantes de `watercube` sur une cadence virtuelle `50 Hz` pour garder la rotation centrale, le ripple et le damping `rok` au rythme du binaire d'origine
- restauration du rasterizer affine d'origine pour les materiaux Java `3` / `259`, afin de rester source-faithful sur `saari`
- verrouillage du rendu texte AWT sur une police monospace explicite et sans anti-aliasing pour garder les ecrans texte coherents entre le launcher source et le build `jpackage`

## Prerequis

- un JDK disponible dans le `PATH`

## Lancement

Depuis la racine du depot :

```bat
run_forward_desktop.bat
```

Sans argument interactif, le launcher desktop ouvre maintenant une petite GUI de demarrage pour choisir :

- `Windowed`
- `Fullscreen`
- `Native 512x256`
- `X2 1024x512`
- `1x1 pixel mode`

Le mode `Fullscreen` garde un fond noir sur tout l'ecran et centre la demo dans la taille choisie.
Le mode `1x1 pixel mode` correspond au flag historique `1x1 1` du binaire Java d'origine. Il est maintenant actif par defaut sur le port desktop.

Options historiques conservees :

```bat
run_forward_desktop.bat nosound 1
run_forward_desktop.bat 1x1 1
run_forward_desktop.bat 1x1 0
run_forward_desktop.bat nosound 1 1x1 1
```

Options desktop supplementaires :

```bat
run_forward_desktop.bat launcher 0 displaymode windowed displayscale 2
run_forward_desktop.bat launcher 0 displaymode fullscreen displayscale 1
```

Parametres :

- `launcher 0` : saute la GUI de demarrage
- `displaymode windowed|fullscreen` : force le mode d'affichage
- `displayscale 1|2` : change uniquement la taille d'affichage de la frame finale
- `1x1 1|0` : force le mode `1x1` actif ou inactif

Le flag historique `1x1 1` continue de piloter la resolution de rendu interne. `1x1 0` force l'ancien mode reduit. `displayscale` agit seulement sur la presentation a l'ecran.

Le script compile `java-desktop/src/main/java` dans `java-desktop/build/classes`, puis lance `ForwardDesktopLauncher` en utilisant `original/forward` comme repertoire de travail pour reutiliser les assets d'origine.

## Packaging Win64

Un workflow `jpackage` est maintenant disponible pour produire un build Windows autonome avec runtime Java embarque :

```bat
package_forward_desktop.bat
```

Sortie par defaut :

```text
java-desktop\dist\jpackage\app-image\Forward\Forward.exe
```

Ce `Forward.exe` n'a pas besoin d'un JDK installe sur la machine cible.

Le build packagé utilise le meme launcher GUI que le build source, avec les memes options `displaymode`, `displayscale` et `launcher 0`.

Installer Windows optionnel :

```bat
package_forward_desktop.bat exe
```

La generation de l'installeur demande WiX dans le `PATH`. Le `app-image` simple, lui, ne depend que du JDK.

Le workflow detaille est documente dans :

- `documentation/forward-jpackage-workflow.md`

## Capture de reference

Le build desktop sait maintenant s'auto-capturer en PNG via des parametres `cle valeur`.

Exemple :

```bat
run_forward_desktop.bat capture documentation\reference-capture\java captureintervalms 2000 capturelimit 60 captureexit 1
```

Le mode capture saute automatiquement la GUI de demarrage. Les captures restent en resolution native `512x256`, meme si l'affichage interactif a ete choisi en `x2`.

Wrappers prets a l'emploi :

```bat
capture_forward_demo.bat
capture_reference_video.bat
```

Sorties :

- `documentation/reference-capture/java/manifest.csv`
- `documentation/reference-capture/java/frames/*.png`

Le workflow complet Java capture + extraction video + comparaison est documente dans :

- `documentation/forward-reference-capture-workflow.md`

## Diagnostic Saari

Une note d'enquete dediee au ciel `saari` est tenue dans :

- `documentation/forward-saari-sky-investigation.md`

Le build desktop expose aussi un switch de diagnostic temporaire pour comparer plusieurs interpretations du mapping du fond :

```bat
set JAVA_TOOL_OPTIONS=-Dforward.saariBackdropUvMode=procedural
```

Valeurs disponibles :

- `procedural`
- `mesh`
- `spherical`

Le mode par defaut reste `procedural`. Ces options servent uniquement a l'enquete de fidelite du ciel `saari`.

Probe numerique disponible :

```bat
probe_saari_sky_original.bat
probe_saari_sky_java_desktop.bat
compare_saari_sky_probe.bat
```

Le workflow et les sorties CSV sont documentes dans :

- `documentation/forward-saari-probe-workflow.md`

Resultat actuellement confirme pour `SCENE_TIME_MS=144000` en mode `procedural` :

- les UVs, sommets projetes et triangles visibles du backdrop `saari` sont identiques entre `original` et `java-desktop`
- un vrai ecart de reconstruction a ete corrige dans `kaajmma.MajAkKa(float)` pour retrouver la semantique bytecode `f2l; l2i`
- le `backdrop_raster_preview.png` du probe est maintenant identique pixel par pixel entre `original` et `java-desktop` au checkpoint `144000 ms`
