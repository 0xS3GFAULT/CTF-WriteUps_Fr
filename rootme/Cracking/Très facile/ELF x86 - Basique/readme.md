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

La commande **size** va nous permettre de savoir la taille des différentes sections de notre programme. Ainsi, nous pourrons savoir si cette dernière contient de la donnée initialisée :

![Screenshot](./assets/images/size_command_ch1.png?raw=true)

La section **.data** contient 296 octets de données initialisées. Peut-être que quelques-uns de ces octets représentent le mot de passe hardcodé dans une variable ? 

Pour nous en convaincre, utilisons la commande **strings** afin de filtrer toutes les chaînes de caractères présentes dans le programme.

![Screenshot](./assets/images/strings_command_ch1.png?raw=true)

Nous y voyons << **Bien joue, vous pouvez valider l'epreuve avec le pass : %s!** >> ainsi qu'une chaîne qui s'apparente à un mot de passe relativement peu protégé : << **123456789** >>. Est-ce bien le bon mot de passe ? Il n'y a qu'un moyen de le savoir...

![Screenshot](./assets/images/flag_ch1.png?raw=true)

Bingo ! Le challenge porte bien son nom...
