# Forward Java Desktop

Base de reconstruction Java desktop de `forward`, derivee des sources decompilees de `reverse/cfr_single`.

## Ce qui a ete modernise

- suppression de la dependance `Applet` au profit d'un hote desktop AWT
- remplacement des anciens backends audio `IE3/IE4` / `sun.audio` par `Java Sound`
- correction des classes decompilees qui contenaient encore des artefacts `GOTO`
- desactivation du basculement intermediaire vers une fenetre ecran entier pour garder toute la demo dans la meme fenetre desktop
- recalage du scroll `phorward.gif` de `domina` et `uppol` sur une cadence virtuelle `50 Hz` pour eviter l'acceleration sur machines modernes
- restauration du rasterizer affine d'origine pour les materiaux Java `3` / `259`, afin de rester source-faithful sur `saari`

## Prerequis

- un JDK disponible dans le `PATH`

## Lancement

Depuis la racine du depot :

```bat
run_forward_desktop.bat
```

Options historiques conservees :

```bat
run_forward_desktop.bat nosound 1
run_forward_desktop.bat 1x1 1
run_forward_desktop.bat nosound 1 1x1 1
```

Le script compile `java-desktop/src/main/java` dans `java-desktop/build/classes`, puis lance `forward` en utilisant `original/forward` comme repertoire de travail pour reutiliser les assets d'origine.

## Capture de reference

Le build desktop sait maintenant s'auto-capturer en PNG via des parametres `cle valeur`.

Exemple :

```bat
run_forward_desktop.bat capture documentation\reference-capture\java captureintervalms 2000 capturelimit 60 captureexit 1
```

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
- la prochaine cible d'investigation doit donc etre le rasterizer affine (`kaajmma`) et non la projection du dome
