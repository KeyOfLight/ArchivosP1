#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <fstream>
#include "./Analizar/Analizador.cpp"

using namespace std;


int main();

int main(){
    Ann analisis;

    int opcion;

    cout << "*********MENU GENERAL**********" << endl
        << "*****Diego Andre Gomez 201908327*****" << endl
        << "*1. Ingresar comando*" << endl
        << "*******************************" << endl;

    string Entrada; //Comando que se desea ejecutar
    cout << "Escriba el comando que desea ejecutar" << endl;

    getline(cin,Entrada);

    analisis.Analizar(Entrada);

}