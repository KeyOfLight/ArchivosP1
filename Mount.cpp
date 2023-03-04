#include <iostream>
#include "Estructuras.cpp"
#include <bits/stdc++.h>
#include "Compartido.cpp"
#include <fstream>
#include <vector>
using namespace std;

class Mount{
    public:

    Mounter Montar(string path, string nombre);
}

Mounter Mount::Montar(string path, string nombre){

    string path = parameters.direccion;
    Particion empt;
    MBR mbrActual;
    FILE* dsk = fopen(path.c_str(), "rb+");
    rewind(dsk);
    fread(&mbrActual, sizeof(mbrActual), 1, dsk);
    int extstart = 0;
}