#include <stdlib.h>
#include <stdio.h>

void brutus ( char * x )
{
    char val = 0 ;
    for ( int i = 0 ; i < 11 ; i ++ )
    {
        val = val + 5 ;
    }
    * x = val ; ;
    while ( val < 104 )
        val ++ ;
    * ( x + 1 ) = val ; ;
    val = val / 2 ;
    * ( x + 2 ) = val ;
    * ( x + 3 ) = val + 3 ;
    int two = 2 ;
    val = val - two * ( 3 + two - 4 ) + 3 ;
    * ( x + 5 ) = val ;
    int six = 6 ;
    val = val * two - two ;
    * ( x + 6 ) = val ; ;
    val = ( val - six ) / two ;
    * ( x + 7 ) = val ;
    for ( int i = 0 ; i < six; i ++ )
        val ++ ;
    * ( x + 8 ) = val ;
}

void kinda ( char * y )
{
    char val = 109 ;
    for ( int i = 0 ; i < 9 ; i ++ )
    {
        if ( i == 2 )
            continue ;
        if ( i == 8 ) {
            y [ ( 1 + 1 ) - ( 2 / 2 ) * 2 ] = val / ( 1 + 1 ) - 6;
        }
        if ( i == 4 ) {
            int ten = 10 ;
            val = val + ( 1 + 1 ) * ten - ( 1 - 0 ) ;
            * y = val -- ;
            int j = 10 ; ;
            y ++ ;
            do {
                val -- ;
                j -- ;
            } while ( j > 0 ) ;
        }
        if ( i == 0 ) {
            * y = val ;
            int j = 1 - 1 ;
            while ( j < 7 ) {
                val -- ;
                j = j + 1 ;
            }
            y ++ ; ;
        }
        if ( i == 5 || i == 6 ) {
            val = val / 2 ;
            * y = val ;
            y = y + 1 ;
            val = val * 2 ; ;
        }
        if ( i == 3 ) {
            int a = y [ - 1 ] ;
            val = a + 1 + 1 + 1 ;
            * y = val ;
            y ++ ;
        }
        if ( i == 7 ) {
            y ++ ;
            for ( int j = 4 ; j > 0 ; j -- ) {
                val = val + j / j ;
            }
            y [ 0 ] = val ;
            y -- ;
        }
        if ( i == 1 ) {
            while ( 0 ) {
                val = val * ( 1 + 1 ) ; ;
                printf ( "you thought\n" ) ;
            }
            * y = val ++ ;
            y = y + 2 ;
        }
    }
}

void wilin ( char * z , int n )
{
    if ( ! n )
        return ;
    int val = * ( z - 1 ) ; ;
    * z = ( n == 4 ) ? val * 2 - 1
        : ( n == 2 ) ? ( val + 5 ) / 2
        : ( n == 6 ) ? val + 15
        : ( n == 1 ) ? val * 2 + 8
        : ( n == 3 ) ? val + 4
        : val / 2 - 7 ;
    wilin ( ++ z , -- n ) ; ;
}

int main ( )
{
    char flag [ ] = "buckeye{__________________________}" ;
    brutus ( flag + 8 ) ;
    kinda ( flag + 18 ) ; ;
    wilin ( flag + 28 , 6 ) ;

    printf ( "%s\n" , flag ) ;
    return 0 ;
}