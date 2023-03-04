#pragma once
#include <iostream>
#include "Estructuras.cpp"

using namespace std;

class Compartido{
public:
    int VerFit(string Actualfit);
    int realsize(int tam, string unidad);
};


int Compartido::VerFit(string Actualfit){

    char temp = Actualfit.back();

    if (temp == ' ')
        Actualfit.pop_back();

    Fits fit;

    for (int i = 0; i < 3; i++)
    {
        if (fit.FitType[i] == Actualfit){
            return i;
        }
        
    }
    return 1;
};

int Compartido::realsize(int tam, string unidad){

    int newsize;
    if(unidad.find("k") == 0){
        newsize = tam * 1024;
    }else if(unidad.find("m") == 0){
        newsize = tam * 1024 * 1024;
    }else {
        newsize = tam * 1024 * 1024;
    }

    return newsize;
};