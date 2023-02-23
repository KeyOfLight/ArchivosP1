#include <iostream>
#include "Estructuras.cpp"

using namespace std;

class Compartido{
public:
    int VerFit(string Actualfit);
};


int Compartido::VerFit(string Actualfit){

    Fits fit;

    for (int i = 0; i < sizeof(fit.FitType); i++)
    {
        if (fit.FitType[i] == Actualfit){
            return i;
        }

        return 1;
        
    }
    
    
}