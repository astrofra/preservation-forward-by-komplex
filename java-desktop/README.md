# Forward Java Desktop

Base de reconstruction Java desktop de `forward`, dérivée des sources décompilées de `reverse/cfr_single`.

## Ce qui a été modernisé

- suppression de la dépendance `Applet` au profit d’un hôte desktop AWT
- remplacement des anciens backends audio `IE3/IE4` / `sun.audio` par `Java Sound`
- correction des classes décompilées qui contenaient encore des artefacts `GOTO`

## Prérequis

- un JDK disponible dans le `PATH`

## Lancement

Depuis la racine du dépôt :

```bat
run_forward_desktop.bat
```

Options historiques conservées :

```bat
run_forward_desktop.bat nosound 1
run_forward_desktop.bat 1x1 1
run_forward_desktop.bat nosound 1 1x1 1
```

Le script compile `java-desktop/src/main/java` dans `java-desktop/build/classes`, puis lance `forward` en utilisant `original/forward` comme répertoire de travail pour réutiliser les assets d’origine.
