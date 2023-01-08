# ELF x86 - Fake Instructions

| Auteur/Autrice | Difficulté | Points |
|----------------|------------|--------|
|     [kmkz](https://www.root-me.org/kmkz?lang=fr)       |Facile|   15    |     

## Sommaire
- [Partie 1 : Analyse de l'exécution](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/Cracking/Facile/ELF%20x86%20-%20Fake%20Instructions/readme.md#partie-1--analyse-de-lex%C3%A9cution)
- [Partie 2 : Analyse statique](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/Cracking/Facile/ELF%20x86%20-%20Fake%20Instructions/readme.md#partie-2--analyse-statique)
- [Partie 3 : Analyse dynamique](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/Cracking/Facile/ELF%20x86%20-%20Fake%20Instructions/readme.md#partie-2--analyse-statique)

## Partie 1 : Analyse de l'exécution

Après avoir donné les droits d'exécution, exécutons le programme [crackme](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/Cracking/Facile/ELF%20x86%20-%20Fake%20Instructions/crackme) : 

![Screenshot](./assets/images/exec_ch4.png?raw=true)

On nous demande de saisir un argument à la commande. Nous saisissons alors **azerty** puis le programme se ferme avec un message qui nous indique de trouver le bon mot de passe. 

Analysons le programme.

## Partie 2 : Analyse statique

Utilisons la commande **file** pour connaître les caractéristiques de ce fichier exécutable : 

![Screenshot](./assets/images/file_ch4.png?raw=true)

Nous pouvons y lire **not stripped** ce qui signifie que les informations de debug sont toujours présentes. 

Y-a t'il une fonction **main** ?  Vérifions-le à l'aide de la commande ```objdump -D -M intel crackme --disassemble=main``` : 

![Screenshot](./assets/images/objdump_ch4_1.png?raw=true)
![Screenshot](./assets/images/objdump_ch4_2.png?raw=true)

Très bien, nous avons un point d'entrée. En analysant rapidement la fonction, nous ne voyons pas d'appel de fonction relatant une saisie d'utilisateur, ni une comparaison sur des chaînes de caractères...

On y voit du **memcpy**, du **malloc**, du **printf**... Mais à l'adresse **0x80486a4** nous avons ceci :

```80486a4:	ff d2                	call   edx```

Le registre **EDX** est chargé d'une adresse d'une certaine fonction qui sera ensuite appelée. Mais quelle est cette fonction ? Utilisons **gdb-peda** pour le savoir.

## Partie 3 : Analyse dynamique

Plaçons un breakpoint à l'adresse **0x80486a4** et lançons le programme : 

![Screenshot](./assets/images/gdb_ch4_1.png?raw=true)

Nous pouvons voir ceci : ```EDX: 0x80486c4 (<WPA>:	push   ebp)```

Le registre **EDX** contient l'adresse de la fonction **WPA**, qui prends deux arguments : notre chaîne de caractères **_azerty_** et une autre chaîne particulière **_\_0cGjc5m\_.5T3Ç8CJ0À9_**

Désassemblons cette fonction **WPA** : 

![Screenshot](./assets/images/gdb_ch4_2.png?raw=true)

Oh, voilà quelque chose d'intéressant ! ```0x080486f5 <+49>:	call   0x804847c <strcmp@plt>``` va comparer l'argument de la commande avec une chaîne de caractères nouvellement créée.

Si la comparaison est bonne, alors une nouvelle fonction nommée **blowfish** est exécutée, puis le programme s'arrête. Sinon, c'est la fonction **RS4** qui est exécutée.

Mais que font ces deux fonctions ? Nous supposons que **blowfish** est la fonction qui valide le mot de passe, et que **RS4** est la fonction qui affiche le message d'erreur si le mot de passe est incorrect, à la suite de la comparaison avec **strcmp**. Appelons **RS4** : 

![Screenshot](./assets/images/gdb_ch4_3.png?raw=true)

Notre hypothèse est validée ! Donc si nous exécutons **blowfish**...

![Screenshot](./assets/images/gdb_ch4_4.png?raw=true)

nous tombons nez-à-nez avec le flag ! Youpi !


