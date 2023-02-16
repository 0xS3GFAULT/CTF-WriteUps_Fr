# ELF x86 - Stack buffer overflow basic 3

| Auteur/Autrice | Difficulté | Points |
|----------------|------------|--------|
|     [Lyes](https://www.root-me.org/Lyes?lang=fr)       | Moyen|   25    |     

## Sommaire
- [Partie 1 : Analyse de l'exécution](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/App%20-%20Syst%C3%A8me/Moyen/ELF%20x86%20-%20Stack%20buffer%20overflow%20basic%203/readme.md#partie-1--analyse-de-lex%C3%A9cution)
- [Partie 2 : Analyse statique du code](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/App%20-%20Syst%C3%A8me/Moyen/ELF%20x86%20-%20Stack%20buffer%20overflow%20basic%203/readme.md#partie-2--analyse-statique-du-code)
- [Partie 3 : Exploitation du buffer overflow](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/App%20-%20Syst%C3%A8me/Moyen/ELF%20x86%20-%20Stack%20buffer%20overflow%20basic%203/readme.md#partie-3--exploitation-du-buffer-overflow)

## Partie 1 : Analyse de l'exécution

Avant d'exécuter le programme **./ch16**, nous remarquons plusieurs informations nous donnant une vérification de sécurité (checksec) du programme : 

![Screenshot](./assets/images/checksec_ch16_1.png?raw=true)
![Screenshot](./assets/images/checksec_ch16_2.png?raw=true)

Pas de **canary**, pas d'**ASLR** ni de **PIE**. L'exécutable est codé sur 32 bits, donc les adresses sont codées sur 4 octets.

Exécutons le programme : 

![Screenshot](./assets/images/exec_ch16_1.png?raw=true)

Le programme nous demande notre nom. Nous tapons *azerty* puis il redemande une saisie en boucle. Apparemment, le mot *azerty* n'aide pas puisque le programme ne fait rien d'autre...

Nous n'avons pas spécialement beaucoup d'informations, mais le code source en langage C du programme compilé nous est donné ! Analysons-le.

## Partie 2 : Analyse statique du code

![Screenshot](./assets/images/code_ch16_1.png?raw=true)
![Screenshot](./assets/images/code_ch16_2.png?raw=true)

4 éléments sont déclarés dans la fonction **main**, dont 2 sont initialisés :

```
char buffer[64];
int check;
int i = 0;
int count = 0;
```

Voici la représentation simplifiée de la pile : 

<pre>
----------
| count  | (4 octets et égal à 0)
----------
|   i    | (4 octets et égal à 0)
----------
| check  | (4 octets)
----------
| buffer | (64 octets)
----------
|  ebp   | (4 octets)
----------
</pre>

En analysant rapidement le code, nous comprenons que si ```check == 0xbffffabc```, le shell **/bin/bash** se lance et nous gagnons la partie.
Cependant le buffer overflow n'est pas possible puisque ```read(fileno(stdin),&i,1);``` (à la ligne 27) ne lit qu'octet par octet l'entrée standard **stdin** et l'adresse de **buffer** est supérieure à l'adresse de **check**.

Il faut alors accéder à la mémoire précédant l'adresse du buffer, et pouvoir réecrire ces 4 cases : 

```buffer[-1], buffer[-2], buffer[-3], buffer[-4]```

... et c'est possible grâce au switch de la ligne 33 !

<pre>
case 0x08:
    count--;
    printf("\b");
    break;
</pre>

Si l'octet saisi vaut **0x08**, la variable **count** est décrémentée. Cette même variable peut être utilisée comme indice du tableau **buffer** à la ligne 45, uniquement si toutes les conditions du switch ne sont pas vérifiées :

<pre>
default:
    buffer[count] = i;
    count++;
    break;
</pre>

Voilà notre plan : écrire 4 fois la valeur **0x08** pour ensuite insérer la valeur **0xbffffabc** de telle sorte à ce que : 

```buffer[-1] = 0xbc, buffer[-2] = 0xfa, buffer[-3] = 0xff, buffer[-4] = 0xbf```

## Partie 3 : Exploitation du buffer overflow

En utilisant Python, nous pouvons écrire le payload suivant : ```(python -c "print('\x08'*4+'\xbc\xfa\xff\xbf')";cat -) | ./ch15```

Il ne faut pas oublier le ```cat -``` qui bloque stdin afin que nous puissons rester dans le shell nouvellement crée.

![Screenshot](./assets/images/exploit_ch16.png?raw=true)

Nous avons gagné la partie :)
