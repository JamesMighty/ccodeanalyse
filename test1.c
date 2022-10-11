countingblank()// Author: Simon Powers
// Date of last modification: 13/02/2022
// Test 1 for Coursework 1.

#include <stdio.h>

// Adds two integers.
// Parameters:
// x : int to add
// y : int to add
int add ( int x , int y )
{
	return x + y ;
}

// Subtract two integers.
// Parameters:
// x : int to subtract
// y : int to subtract
int sub ( int x , int y )
{
	return x - y ;
}

int main ( int argc , char * argv [] )
{
	int a = 1 ;
	int b = 2 ;
	int c = add ( a , b ) ;
	int d = sub ( a , b ) ;
	printf ( "a + b = %d\n" , c ) ;
	printf ( "a - b = %d\n" , d ) ;
	return 0 ;
}