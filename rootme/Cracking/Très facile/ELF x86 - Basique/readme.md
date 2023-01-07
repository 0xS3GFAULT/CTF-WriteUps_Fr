# ELF x86 - Basique

| Auteur/Autrice | Difficulté | Points |
|----------------|------------|--------|
|     [g0uZ](https://www.root-me.org/g0uZ?lang=fr)       | Très facile|   5    |     

## Sommaire
- [Partie 1 : Analyse de l'exécution](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/Cracking/Tr%C3%A8s%20facile/ELF%20x86%20-%20Basique/readme.md#partie-1--analyse-de-lex%C3%A9cution)
- [Partie 2 : Analyse statique](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/Cracking/Tr%C3%A8s%20facile/ELF%20x86%20-%20Basique/readme.md#partie-1--analyse-statique)
- [Partie 3 : Cracking de l'exécutable](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/Cracking/Tr%C3%A8s%20facile/ELF%20x86%20-%20Basique/readme.md#partie-3--cracking-de-lex%C3%A9cutable)

## Partie 1 : Analyse de l'exécution

Après avoir donné les droits d'exécution, exécutons le programme [ch2.bin](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/Cracking/Tr%C3%A8s%20facile/ELF%20x86%20-%20Basique/ch2.bin) : 

![Screenshot](./assets/images/exec_ch2.png?raw=true)

On nous demande de saisir un nom d'utilisateur. L'utilisateur **_azerty_** n'existe pas... Et le programme se ferme. 

Analysons alors son contenu.

## Partie 2 : Analyse statique

Utilisons la commande **file** pour connaître les caractéristiques de ce fichier exécutable : 

![Screenshot](./assets/images/file_ch2.png?raw=true)

Nous pouvons y lire **not stripped** ce qui signifie que les informations de debug sont toujours présentes: nous devrions y trouver des noms de fonctions et de variables, qui ont été sûrement initialisées.

Cependant, nous ne pouvons pas utiliser la commande **strings** comme au challenge [ELF x86 - 0 protection](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/tree/main/rootme/Cracking/Tr%C3%A8s%20facile/ELF%20x86%20-%200%20protection) puisqu'il y a bien trop de données à scanner. Tentons d'analyser le code de la section **.text**.

Avec l'outil **objdump**, il est possible de désassembler le code exécutable. Si le programme a été compilé avec **gcc**, c'est qu'il y a forcément une fonction **main**. Vérifions : 

![Screenshot](./assets/images/objdump_ch2_1.png?raw=true)
![Screenshot](./assets/images/objdump_ch2_2.png?raw=true)

Une fonction **main** existe bien ! C'est notre point d'entrée du programme.

En analysant légèrement la fonction, nous pouvons faire plusieurs hypothèses.

```804835e:	e8 07 ff ff ff       	call   804826a <getString>```

```804838e:	e8 d7 fe ff ff       	call   804826a <getString>``` 

Ces deux lignes représentent les appels de la fonction **getString** qui pourrait nous servir à saisir respectivement le nom d'utilisateur et probablement son mot de passe. 

```8048373:	e8 78 7f 00 00       	call   80502f0 <strcmp>```

```80483a3:	e8 48 7f 00 00       	call   80502f0 <strcmp>```

Ces deux lignes représentent les appels de la fonction **strcmp** qui nous servirait à comparer respectivement le nom d'utilisateur et le mot de passe avec d'autres chaînes de caractères. Si la comparaison est bonne, **strcmp** retourne 0 dans le registre **RAX**. Sinon, **strcmp** retourne un nombre négatif ou positif différent de 0 dans **RAX**. 

Il suffit alors de duper la machine en esquivant cette comparaison. Plusieurs possibilités s'offrent à nous, telles que : 
- Modifier le registre **RAX** en lui donnant la valeur 0 après l'appel de **strcmp**
- Remplacer les sauts conditionnels **jne** *(jump if not equal)* à la suite des tests de **RAX** par des **NOP** (d'opcode **0x90**)

Nous allons alors choisir la seconde option et utiliser l'outil **ghex** pour modifier les données.

##  Partie 3 : Cracking de l'exécutable
Voici les deux instructions à remplacer par des **NOP**:

```804837a:	75 54                	jne    80483d0 <main+0xc7>```

```80483aa:	75 16                	jne    80483c2 <main+0xb9>```

Ces deux instructions font 2 octets chacun, il va donc falloir écrire 2 **NOP** à l'adresse **0x804837a** et 2 **NOP** à l'adresse **0x80483aa**.

![Screenshot](./assets/images/ghex_ch2.png?raw=true)
