# ELF x86 - Stack buffer overflow basic 1

| Auteur/Autrice | Difficulté | Points |
|----------------|------------|--------|
|     [Lyes](https://www.root-me.org/Lyes?lang=fr)       | Très facile|   5    |     

## Sommaire
- [Partie 1 : Analyse de l'exécution]()
- [Partie 2 : Analyse statique du code]()
- [Partie 3 : Exploitation du buffer overflow]()

## Partie 1 : Analyse de l'exécution

Avant d'exécuter le programme **./ch13**, nous remarquons plusieurs informations nous donnant une vérification de sécurité (checksec) du programme : 

![Screenshot](./assets/images/exec_ch13_1.png?raw=true)

Nous remarquons que la sécurité est légère : aucun canary, pas d'ASLR ni de PIE.

Exécutons le programme : 

![Screenshot](./assets/images/exec_ch13_2.png?raw=true)

Le programme nous demande de saisir quelque chose. Nous tapons '**azerty**', puis le programme nous retourne deux informations. Le buffer '**buf**' vaut **azerty** et l'élément '**check**' vaut **0x4030201**. Puis le programme se ferme...

Nous n'avons pas spécialement beaucoup d'informations, mais le code source en langage C du programme compilé nous ai donné ! Analysons-le.

## Partie 2 : Analyse statique du code

![Screenshot](./assets/images/code_ch13_1.png?raw=true)

Nous nous rendons compte assez rapidement que le but du jeu est de modifier la variable **check** et de la passer à la valeur **0xdeadbeef**.

La ligne 24 du code ```setreuid(geteuid(), geteuid())``` permet de modifier l'UID réel et effectif du processus appelant. C'est ce qui va nous permettre par la suite de lancer le shell **/bin/bash** avec les mêmes droits que le possesseur du programme. Mais à quoi peut bien nous servir ce shell ? A lire le fichier caché **~/.passwd** qui doit surement contenir le flag...

Bon et du coup, comment modifions-nous la variable **check** ? Le programme nous demande de saisir quelque chose au clavier et se contente de l'afficher : nous n'avons aucun contrôle sur les autres variables ! A moins que... nous puissions modifier la pile directement à partir de la chaîne de caractères saisie à la manière d'un buffer overflow ?

Reprenons les 3 variables mises en jeu : 
```
int var;
int check = 0x04030201;
char buf[40];
```

En redessinant grossièrement la pile de la fonction **main** à la ligne 11, nous avons ceci : 

![Screenshot](./assets/images/pile_ch13_1.JPG?raw=true)

On y insère les valeurs connues :

![Screenshot](./assets/images/pile_ch13_2.JPG?raw=true)

La fonction ```fgets(buf,45,stdin);``` à la ligne 13 permet de remplir la zone mémoire du buf[40] en position **RBP-0x30** dans la pile de 44 octets maximum (le 45ème octet est pour le **'\0'** de fin de chaîne.) Mais... notre buffer ne fait que 40 octets... Il y a un problème non ? 

En effet, en passant une taille trop grande à **fgets**, cette dernière s'attendra à ce que notre buffer soit plus grand qu'il ne l'est réellement, et pourra réecrire plus haut dans la pile ! Il faut exploiter cette faille là pour produire un buffer overflow et réecrire dans la variable **check** en position **RBP-0x8** (jusqu'à **RBP-0x4** donc les 4 octets de **0xdeadbeef**). 

La fonction **fgets** nous permet d'écrire à partir de la position **RBP-0x30**. Donc Il faudra remplir la pile (par exemple par des **'A'** de code ascii **0x41**) sur une zone de ```0x30 - 0x8 = 40 octets``` afin d'accéder directement à l'injection de **0xdeadbeef** dans la variable **check** : 

![Screenshot](./assets/images/pile_ch13_3.JPG?raw=true)

En utilisant par exemple le langage de programmation Python, nous pouvons construire le payload à injecter dans stdin : ```'A'*40+'\xef\xbe\xad\xde'```

Que représente ```'\xef\xbe\xad\xde'``` ? C'est le mot hexadécimal **0xdeadbeef** qui est écrit à l'envers, car en écrivant dans la pile, on remonte vers **RBP** en little endian (car ici la pile utilise l'ordre little endian). En d'autres termes, à chaque fois que nous voulons écrire une valeur hexadécimale dans la pile, il faudra l'écrire inversée.

Il ne reste plus qu'à exploiter la faille avec Python.

## Partie 3 : Exploitation du buffer overflow

Utilisons le payload en Python suivi d'un pipe afin d'injecter dans stdin ce que l'on souhaite :

![Screenshot](./assets/images/exploit_ch13_1.png?raw=true)

Et ça marche ! Enfin presque... nous avons réussi mais le shell lancé se ferme instantanément, nous ne pouvons pas lire le fichier **.passwd**... Comment faire ? Il suffit de bloquer l'entrée standard stdin avec la commade **cat** suivi d'un tiret, comme tel ``` (commande;cat -) | programme``` : 

![Screenshot](./assets/images/exploit_ch13_2.png?raw=true)

Et voilà ! On peut se ballader dans le shell qui a été lancé, il ne se ferme plus automatiquement car nous avons bloqué stdin. On demande à lire le fichier **.passwd** qui est détenu par *app-systeme-ch13-cracked* mais nous avons tous ses droits grâce à la ligne 24 ```setreuid(geteuid(), geteuid())```, le programme **ch13** est toujours en exécution même si nous sommes dans un nouveau shell. Le flag vient naturellement :)

Une autre possibilité consistait à rediriger la position des lancements de scripts bash en utilisant la variable **$BASH_ENV**. Cette variable d'environnement sert à effectuer des commandes avant de lancer un script, à des fins de débogage par exemple.

Dans cette variable, nous aurions pu y placer le chemin vers un script bash crée dans **/tmp**. Il faut savoir que tout le monde peut créer des fichiers dans le dossier **/tmp**. Que mettons-nous dans ce script bash situé dans **/tmp** ? Eh bien tout simplement la commande **cat .passwd**.

En faisant cela, nous demandons à la machine de lancer le script que nous avons crée dans **/tmp** avant de lancer n'importe quel autre script, notamment celui à la ligne 25 ```system("/bin/bash");```. Voici une démonstration : 

![Screenshot](./assets/images/exploit_ch13_3.png?raw=true)

C'est magique ;)
