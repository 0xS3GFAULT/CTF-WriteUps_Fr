# ELF x86 - Basique

| Auteur/Autrice | Difficulté | Points |
|----------------|------------|--------|
|     [g0uZ](https://www.root-me.org/g0uZ?lang=fr)       | Très facile|   5    |     

## Sommaire
- [Partie 1 : Analyse de l'exécution](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/Cracking/Tr%C3%A8s%20facile/ELF%20x86%20-%200%20protection/readme.md#partie-1--analyse-de-lex%C3%A9cution)
- [Partie 2 : Analyse statique](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/Cracking/Tr%C3%A8s%20facile/ELF%20x86%20-%200%20protection/readme.md#partie-2--analyse-statique)

## Partie 1 : Analyse de l'exécution

Après avoir donné les droits d'exécution, exécutons le programme [ch2.bin](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/Cracking/Tr%C3%A8s%20facile/ELF%20x86%20-%20Basique/ch2.bin) : 

![Screenshot](./assets/images/exec_ch2.png?raw=true)

On nous demande de saisir un nom d'utilisateur. L'utilisateur **_azerty_** n'existe pas... Et le programme se ferme. 

Analysons alors son contenu.

## Partie 2 : Analyse statique

Utilisons la commande **file** pour connaître les caractéristiques de ce fichier exécutable : 

![Screenshot](./assets/images/file_ch2.png?raw=true)

Nous pouvons y lire **not stripped** ce qui signifie que les informations de debug sont toujours présentes: nous devrions y trouver des noms de fonctions et de variables, qui ont été sûrement initialisées.

Cependant, nous ne pouvons pas utiliser la commande **strings** comme au challenge [ELF x86 - 0 protection]() puisqu'il y a bien trop de données à scanner. Tentons d'analyser le code de la section **.text**.

Il faut trouver le point d'entrée du programme. L'outil **gdb-peda** va nous permettre de nous simplifier la tâche avec la commande **info files**:


