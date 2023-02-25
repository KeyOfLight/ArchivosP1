#pragma once
#include <iostream>
#include "Estructuras.cpp"

using namespace std;

class Compartido{
public:
    int VerFit(string Actualfit);
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