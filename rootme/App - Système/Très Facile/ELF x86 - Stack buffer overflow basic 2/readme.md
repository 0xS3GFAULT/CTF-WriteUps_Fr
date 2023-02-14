# ELF x86 - Stack buffer overflow basic 2

| Auteur/Autrice | Difficulté | Points |
|----------------|------------|--------|
|     [Lyes](https://www.root-me.org/Lyes?lang=fr)       | Très facile|   5    |     

## Sommaire
- [Partie 1 : Analyse de l'exécution]()
- [Partie 2 : Analyse statique du code]()
- [Partie 3 : Exploitation du buffer overflow]()

## Partie 1 : Analyse de l'exécution

Avant d'exécuter le programme **./ch15**, nous remarquons plusieurs informations nous donnant une vérification de sécurité (checksec) du programme : 

![Screenshot](./assets/images/checksec_ch15.png?raw=true)

Pas de **canary**, pas d'**ASLR** ni de **PIE**.

Exécutons le programme : 

![Screenshot](./assets/images/exec_ch15_1.png?raw=true)

Le programme attend une saisie. Nous tapons '**azerty**', puis le programme nous retourne '*Hey dude ! Waaaaazzaaaaaaaa ?!*'. Puis le programme se ferme...

Nous n'avons pas spécialement beaucoup d'informations, mais le code source en langage C du programme compilé nous ai donné ! Analysons-le.

## Partie 2 : Analyse statique du code

![Screenshot](./assets/images/code_ch15_1.png?raw=true)

La fonction **main** demande à l'utilisateur de saisir quelque chose avec la fonction **fgets**, et appelle la mystérieuse fonction **func**. En fait, ```void (*func)()=sup;``` (à la ligne 18) est un pointeur de fonction , qui pointe vers l'adresse de la fonction **sup**. C'est donc la fonction **sup** qui va être appelée à la ligne 21. Mais nous voyons rapidement que c'est la fonction **shell** qui nous intéresse, car elle nous permet de lancer le shell **/bin/bash** avec les mêmes droits que le possesseur du programme.

Nous savons ce qu'il nous reste à faire : rediriger le pointeur de fonction **func** vers l'adresse de **shell** pour pouvoir l'appeler ! Mais est-ce vraiment réalisable ? Nous ne contrôlons que les 133 premiers octets en partant de l'adresse de **buffer**, grâce à ```fgets(buf,133,stdin);``` (à la ligne 20). Mais la taille de ```char buf[128];``` (à la ligne 19) est de 128 octets... Il y a donc un buffer overflow envisageable, de ```133 - 128 = 5 octets``` !

La commande **file** nous indique que l'exécutable est sur 32 bits : 

![Screenshot](./assets/images/file_ch15.png?raw=true)

Cela représente donc des adresses mémoires de 4 octets, ce qui est juste assez pour en écrire une dans nos 5 octets de buffer overflow :)

Puisque ```var```, ```func``` et ```buf``` sont juxtaposés dans la mémoire et que ```buf``` vient avant dans la pile, on va pouvoir écrire la nouvelle adresse pointée par ```func``` qui est de 4 octets : c'est l'adresse de la fonction ```shell```. Mais comment trouver cette adresse ? Puisque les protections **ASLR**,**PIE** et **canary** sont désactivées, on peut directement l'avoir en regardant les symboles avec **nm** par exemple : 

![Screenshot](./assets/images/nm_ch15.png?raw=true)

L'adresse à injecter dans le payload du buffer overflow est donc ```0x08048516```. Exploitons la faille !

## Partie 3 : Exploitation du buffer overflow

En utilisant Python, nous pouvons écrire le payload suivant : ```python -c "print('A'*128+'\x16\x85\x04\x08')" | ./ch15```

Il ne reste plus qu'à tester et... une erreur de segmentation ? Oui, c'est normal, **/bin/bash** est invoqué mais se ferme tout de suite après, il faut bloquer l'entrée standard **stdin** pour pouvoir écrire dans le nouveau shell. Il est possible de la bloquer avec ```cat -```.

Le nouveau payload devient alors ```(python -c "print('A'*128+'\x16\x85\x04\x08')";cat -) | ./ch15```

Il n'y a plus qu'à exécuter et ...

![Screenshot](./assets/images/exploit_ch15.png00?raw=true)

Tadaa ! Le shell parle de lui-même :) 
