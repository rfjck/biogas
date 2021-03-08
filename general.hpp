#ifndef GENERAL_H
#define GENERAL_H

#include <iostream>

using namespace std;

#define ABS(X) ( ((X)>0)? (X):(-(X)) )
#define MIN(X,Y) ( ((X)<(Y))? (X):(Y) )
#define MAX(X,Y) ( ((X)>(Y))? (X):(Y) )
#define SIGN(X) ( ((X)>0)? 1: (((X)==0)?0:-1) )
#define SIGNSTR(X) ( ((X)>0)? "+": (((X)==0)?"0":"-") )

inline int modulo(int a, int b){
    const int result = a % b;
    return result >= 0 ? result : result + b;
}

#endif
