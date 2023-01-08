# ELF x86 - Ptrace

| Auteur/Autrice | Difficulté | Points |
|----------------|------------|--------|
|     [g0uZ](https://www.root-me.org/g0uZ?lang=fr)       |Facile|   15    |     

## Sommaire
- [Partie 1 : Analyse de l'exécution](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/tree/main/rootme/Cracking/Facile/ELF%20x86%20-%20Ptrace#partie-1--analyse-de-lex%C3%A9cution)
- [Partie 2 : Analyse statique](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/tree/main/rootme/Cracking/Facile/ELF%20x86%20-%20Ptrace#partie-2--analyse-statique)
- [Partie 3 : Analyse dynamique](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/tree/main/rootme/Cracking/Facile/ELF%20x86%20-%20Ptrace#partie-3--analyse-dynamique)
- [Partie 4 : Cracking de l'exécutable](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/tree/main/rootme/Cracking/Facile/ELF%20x86%20-%20Ptrace#partie-4--cracking-de-lex%C3%A9cutable)

## Partie 1 : Analyse de l'exécution

Après avoir donné les droits d'exécution, exécutons le programme [ch3.bin](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/rootme/Cracking/Facile/ELF%20x86%20-%20Ptrace/ch3.bin) : 

![Screenshot](./assets/images/exec_ch3.png?raw=true)

On nous demande de saisir un mot de passe. Nous saisissons alors **azerty** puis le programme se ferme en nous indiquant que ce n'est pas le bon mot de passe. 

Analysons le programme.

## Partie 2 : Analyse statique

Utilisons la commande **file** pour connaître les caractéristiques de ce fichier exécutable : 

![Screenshot](./assets/images/file_ch3.png?raw=true)

Nous pouvons y lire **not stripped** ce qui signifie que les informations de debug sont toujours présentes. 

Y-a t'il une fonction **main** ?  Vérifions-le à l'aide de la commande ```objdump -D -M intel crackme --disassemble=main``` : 

![Screenshot](./assets/images/objdump_ch3_1.png?raw=true)
![Screenshot](./assets/images/objdump_ch3_2.png?raw=true)

Très bien, nous avons un point d'entrée. En analysant rapidement la fonction, nous ne trouvons pas d'appels de **strcmp** ni dans la fonction **main** ni dans le symbole **_notng**. La fonction **fgets** est appelée à la fin du **main**: 

```8048486:	e8 05 0b 00 00       	call   8048f90 <_IO_fgets>``` 

afin de demander à l'utilisateur de saisir le mot de passe. Une mystérieuse fonction **ptrace** est appelée en début de fonction **main** : 

```8048410:	e8 5b 06 01 00       	call   8058a70 <ptrace>```. 

Nous nous y pencherons plus tard. En fin de fonction **main** nous avons également ceci : 

```804848e:	8d 05 97 84 04 08    	lea    eax,ds:0x8048497
 8048494:	40                   	inc    eax
 8048495:	ff e0                	jmp    eax
 ```

Donc **EAX** est chargé par l'adresse **0x8048497** qui représente l'adresse du symbole **_notng**, et ... on incrémente **EAX** pour ensuite ramener **RIP** à cette adresse ? Mais que représente l'instruction à l'adresse **0x8048498** ?

Pour le savoir, utilisons **gdb-peda**.

## Partie 3 : Analyse dynamique

Plaçons un breakpoint à l'adresse de **main** et lançons le programme : 

![Screenshot](./assets/images/gdb_ch3_1.png?raw=true)

Argh ! Le message '**Debugger detecté ... Exit**' s'affiche avant de stopper notre programme. On nous espionne ??

En fait, il faut reprendre l'appel de la fonction mystérieuse **ptrace** pour le comprendre : 

```8048408:	6a 00                	push   0x0
 804840a:	6a 01                	push   0x1
 804840c:	6a 00                	push   0x0
 804840e:	6a 00                	push   0x0
 8048410:	e8 5b 06 01 00       	call   8058a70 <ptrace>
 8048415:	83 c4 10             	add    esp,0x10
 8048418:	85 c0                	test   eax,eax
 804841a:	79 1a                	jns    8048436 <main+0x46>
 ```

En langage C, ça se traduit littéralement par :

```
if(ptrace(0,0,1,0)>0)
{
	// OK
}
else
{
	// Erreur
}
```

D'après la page **man** de **ptrace**, 

> L'appel système ptrace() fournit au processus parent un moyen de contrôler l'exécution d'un autre processus et d'éditer son image mémoire.

C'est un appel système utilisable uniquement sur Linux. Le premier argument de **ptrace** est 0, ce qui se caractérise par **PTRACE_TRACEME**. La page **man** nous dit : 

> Le processus en cours va être suivi par son père. [...] Un processus ne doit pas envoyer cette requête si son père n'est pas prêt à le suivre. Dans cette requête, pid, addr et data sont ignorés. La requête ci-dessus ne sert que dans le processus fils. [...] Par la suite, pid précise le fils sur lequel agir. [...] Pour les requêtes PTRACE_PEEK*, ptrace() renvoie la valeur réclamée et zéro pour les autres requêtes, ou -1 en cas d'échec en remplissant errno avec le code d'erreur. Comme la valeur renvoyée par une requête PTRACE_PEEK* peut légitimement être -1, il faut vérifier errno après un tel appel pour vérifier si une erreur s'est produite.  

En fait **gdb-peda**, qui est le débogueur, agit comme un processus père du programme **ch3.bin**. **gdb-peda** va s'attacher à un processus (donc son enfant) au début de l'exécution en utilisant l'appel système **ptrace**. Le problème, c'est que seul un processus peut utiliser **ptrace** à la fois, et c'est **gdb-peda** qui l'utilise en premier. Le processus enfant ne pourra donc pas utiliser l'appel système **ptrace** à ce moment-là, d'où le retour à valeur négative de cet appel système.

Il suffit alors de contourner cet appel, en le modifiant par une mise à 0 de **EAX** afin de valider le test à l'adresse **0x8048418**. Utilisons **ghex**.

## Partie 4 : Cracking de l'exécutable

L'appel de **ptrace** à l'adresse **0x8048410** fait 5 octets : ```e8 5b 06 01 00```. On pourrait utiliser l'instruction ```xor eax,eax``` pour mettre le registre **EAX** à 0. Cette instruction ne fait que 2 octets : ```31 c0```. Nous avons donc 2 octets à modifier sur les 5 octets au total. Il va rester 3 octets qui ne nous servirons à rien , nous pourrons les remplacer par des **NOP** (d'opcode **0x90**).

Voilà ce qu'il faudra écrire à l'adresse **0x8048410** : ```31 c0 90 90 90```

![Screenshot](./assets/images/ghex_ch3_1.png?raw=true)

Retournons maintenant sur **gdb-peda**, et relançons le programme depuis l'adresse du **main** :

![Screenshot](./assets/images/gdb_ch3_2.png?raw=true)

Nous avons réussi à esquiver l'appel de **ptrace** et le programme nous demande bien le mot de passe. Continuons notre analyse ligne par ligne : 

![Screenshot](./assets/images/gdb_ch3_3.png?raw=true)

Les trois instructions :

```0x8048497 <main+167>:	mov    eax,0x8bea558a
0x804849c <main+172>:	inc    ebp
0x804849d <main+173>:	hlt
```

ne seront jamais exécutées, et à la place on y exécute ceci :

```0x8048498 <main+168>:	mov    dl,BYTE PTR [ebp-0x16]
0x804849b <main+171>:	mov    eax,DWORD PTR [ebp-0xc]
```

Ce qui va permettre de charger **DL** par le premier caractère du mot de passe saisi, et **EAX** par l'adresse d'une étrange chaîne de caractères **ksuiealohgy** : 

![Screenshot](./assets/images/gdb_ch3_4.png?raw=true)

A partir de ce moment précis, plusieurs comparaisons vont être faites dans le programme, sur différents caractères entre notre saisie et l'étrange chaîne de caractères. Analysons ces comparaisons : 

![Screenshot](./assets/images/gdb_ch3_5.png?raw=true)

Pour que le programme nous affiche le flag, il faut que :
- Le premier caractère de notre saisie corresponde avec le 5ème caractère de l'étrange chaîne : '**e**'
- Le second caractère de notre saisie corresponde avec le 6ème caractère de l'étrange chaîne : '**a**'
- Le troisième caractère de notre saisie corresponde avec le 2ème caractère de l'étrange chaîne : '**s**'
- Le quatrième caractère de notre saisie corresponde avec le dernier caractère de l'étrange chaîne : '**y**'

Nous supposons alors que le mot de passe est '**easy**'. Vérifions : 

![Screenshot](./assets/images/flag_ch3_5.png?raw=true)

Bingo ! Le mot de passe saisi est notre flag.
