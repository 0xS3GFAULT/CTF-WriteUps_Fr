# CAP

| Auteur/Autrice | Difficulté | Points |
|----------------|------------|--------|
|     [gsemaj](https://github.com/gsemaj)       | facile |   101    |     

Un fichier [cap.c](https://github.com/0xS3GFAULT/CTF-WriteUps_Fr/blob/main/buckeyeCTF2022/rev/facile/cap/cap.c) nous est donné. Oulala mais c'est incompréhensible !

Bon, déjà, nous savons que c'est du langage C. A nous de déterminer la valeur de chacune de ces constantes de préprocesseur... Nous ne ferons que des suppositions et nous vérifierons à la fin la véracité de nos hypothèses.

Si c'est du langage C, nous savons que chaque instruction se termine par un point virgule, et beaucoup de lignes se terminent par ```fr```. Nous supposons alors que ```#define fr ;```

Nous nous rendons compte également que beaucoup de paragraphes existent, commencent par une ligne incompréhensible suivie du mot ```finna``` et se finissent par ```tho```. Ces paragraphes représentent très probablement des fonctions, d'où s'en suivent les expressions ```#define finna {``` et ```#define tho }```

Regardons de plus près la ligne ```sheeeesh ongod "%s\n" bruh flag af ;```. Le **"%s\n"** pourrait nous faire penser à un drapeau de la fonction **printf** ou **scanf** par exemple. Supposons alors que ```sheeeesh ongod "%s\n" bruh flag af ;``` <=> ```printf("%s\n",flag)``` ce qui nous mène à : 

<pre>
#define sheeeesh printf
#define ongod (
#define bruh ,
#define af )
</pre>

Apres avoir fait cela, nous pouvons apercevoir ceci : ```poppin ( lit i lookin cap ; i lowkey 11 ; i playin )```. En toute logique, avec les parenthèses et les points virgules à la chaine, ça pourrait ressembler à une boucle **for**, que nous écririons comme telle : ```for(int i = 0 ; i < 11 ; i ++)``` en assumant que ```lowkey``` corresponde au signe *strictement inférieur à*. Nous avons donc : 

<pre>
#define poppin for
#define lit int
#define looking =
#define cap 0
#define lowkey <
#define playin ++
</pre>

Nous retrouvons à la fin de la fonction **main** ceci : ```deadass 0 ;```, ce qui correspondrait à ```return 0;```. Ainsi, ```#define deadass return```.

Toujours dans la fonction **main**, il y aurait la présence d'une déclaration et d'une initialisation de la chaîne de caractères représentant le flag : ```clean flag yeet rn = "buckeye{__________________________}" ;```. Il serait judicieux de l'écrire ainsi : ```char flag [] = "buckeye{__________________________}" ;``` ce qui nous donne : 

<pre>
#define clean char 
#define yeet [ 
#define rn ] 
</pre>

Puisque nous supposons que ```deadass``` <=> ```return```, nous voyons qu'il n'y a aucun **return** ni dans la fonction **kinda** ni dans **brutus**. De plus, nous avons ```return ;``` dans la fonction **willin** :  nous en déduisons alors que ces trois fonctions sont de type **void** ce qui nous mène à ```#define legit void```.

Dans la fonction **kinda** nous voyons ceci :

<pre>
respectfully {
    val downbad ;
    j downbad ;
} boutta ( j highkey 0 ) ;
</pre>

ce qui s'apparenterai à une syntaxe ```do{...}while(...);```. Nous avons précédemment supposé ```#define lowkey <``` donc par pure logique, ```#define highkey >```. Supposons également que ```downbad``` <=> ```--```. Nous verrons là où ça nous mène, et nous pouvons écrire :

<pre>
#define respectfully do
#define downbad --
#define boutta while
#define highkey >
</pre>

Une des lignes de la fonction **kinda** apparaît comme telle : ```for ( int j = 4 ; j > 0 ; j -- ) {``` et ça a du sens. Nous pouvons être sur et certain que ```#define downbad --```.

Les lignes ```void brutus ( char mf x )``` et ```mf ( x dub 2 ) = val ;``` nous feraient penser que **x** est un (**char \***). Supposons que ```#define mf *```.

Soyons fou, estimons également que dans la fonction **willin**, ```tryna ( no n )``` <=> ```if ( ! n )```. En effet, ```no ~ not ~ not equal to``` donc :

<pre>
#define tryna if
#define no !
</pre>

Faisons de même pour le mot clé **lackin** où ```lackin ~ lack``` en supposant ```#define lackin -```.

Le mot ```dub``` apparaît plusieurs fois, notamment dans la fonction **main** : ```brutus ( flag dub 8 ) ;``` <=> ```brutus ( flag + 8 )```. Cela pourrait faire référence au 8ème octet du tableau de caractères **flag** qui justement représente le début de la partie cachée du flag. Puisque nous avons déjà défini la soustraction, il semblerait logique que ```#define dub +```.

Nous retrouvons aussi ```if ( i be 5 like i be 6 ) ``` et nous pourrions croire ceci : ```if ( i == 5 || i == 6)``` puisqu'il est impossible en utilisant l'opérateur logique **and** que la variable **i** soit à la fois égale à 5 et à 6. Nous en déduisons : 

<pre>
#define be ==
#define like ||
</pre>

Juste en dessous apparaît ceci : ```val = val wack 2 ;```. Nous savons que ```wack``` est une opération mathématique. Nous avons déjà utilisé l'addition, la soustraction et la multiplication. Par déduction, ```#define wack /```.

Bon, le code prend du sens petit à petit, mais le mot ```bussin``` est utilisé plusieurs fois et nous gêne. Quelle est sa valeur ? Il suffit de regarder les conditions de la fonction **kinda** pour le savoir. La variable **i** prend les valeurs de 0 à 8 dans la boucle **for**, et nous voyons les conditions défiler dans l'ordre : ```if ( i == 2 )```, ```if ( i == 8 )```, ```if ( i == 4 )```, ```if ( i == 0 )```, ```if ( i == 5 || i == 6 )```, ```if ( i == 3 )``` et ```if ( i == 7 )```. Il manque le chiffre 1. Il n'y a alors plus aucun doute : ```#define bussin 1```.

Dans la fonction **willin**, nous voyons ceci : 

<pre>
* z = ( n == 4 ) sus val * 2 - 1
    drip ( n == 2 ) sus ( val + 5 ) / 2
    drip ( n == 6 ) sus val + 15
    drip ( n == 1 ) sus val * 2 + 8
    drip ( n == 3 ) sus val + 4
    drip val / 2 - 7 ;
</pre>

Cette syntaxe n'est pas très familière mais il est possible que ça soit une condition ternaire relativement longue. Nous pourrions l'écrire comme ceci :

<pre>
* z = ( n == 4 ) ? val * 2 - 1
    : ( n == 2 ) ? ( val + 5 ) / 2
    : ( n == 6 ) ? val + 15
    : ( n == 1 ) ? val * 2 + 8
    : ( n == 3 ) ? val + 4
    : val / 2 - 7 ;
</pre>

Ce qui nous amène à :

<pre>
#define drip :
#define sus ?
</pre>

Plus que deux constantes à définir : ```yikes``` et ```chill```. Cependant, ```yikes``` n'est utilisée nulle part dans le code : nous pouvons la supprimer. Il ne reste que la constante ```chill``` dans la fonction **kinda**. Il se pourrait que ce soit le mot clé ```continue``` ou ```break``` mais il ne serait pas logique que la boucle **for** s'arrête au bout de la 2ème itération. Nous supposons alors ```#define chill continue```

Nous avons reformé entièrement le code, qui se trouve [juste ici]().

Maintenant il faut savoir si ça fonctionne... Compilons le fichier avec gcc : ```gcc cap.c -o cap``` et croisons les doigts :

![Screenshot](./assets/images/flag_cap.png?raw=true)

Fr that shit mf bussin no cap :)
