#include <iostream>
#include <bits/stdc++.h>
#include "Compartido.cpp"
#include <fstream>
#include <vector>
#include "Estructuras.cpp"

using namespace std;

class Mount{
public:
    Mounter Montar(string path, string nombre);
    Mounter MontarEBR(string dir, string nombre, int startpoint, int ite);
    std::vector<Mounter> Unmount(string id, vector<Mounter> Montados);


    Compartido compart;

};

Mounter Mount::Montar(string path, string nombre){

    FILE* dsk = fopen(path.c_str(), "rb+");
    MBR mbrActual;
    Mounter Montado;
    rewind(dsk);
    fread(&mbrActual, sizeof(mbrActual), 1, dsk);
    string s_a;
    int extstart = 0;

    for (int i = 0; i < 4; i++){
        if(mbrActual.particiones[i].status =='1'){
            int tam = nombre.length();
            s_a = compart.convertToString(mbrActual.particiones[i].name, tam);
            if(mbrActual.particiones[i].type == 'e'){
                extstart = mbrActual.particiones[i].start;
            }
            if(s_a == nombre){
                Montado.id = "27"+i+nombre;
                Montado.name = nombre;
                Montado.path = path;
                Montado.startpoint = mbrActual.particiones[i].start;

                cout<<"El id de la particion es: " + Montado.id <<endl;
                return Montado;
            }
        }
    }

    if(extstart != 0){
        return MontarEBR(path, nombre, extstart, 1);
    }

    return Montado;
}

Mounter Mount::MontarEBR(string dir, string nombre, int startpoint, int ite){
    string path = dir;
    EBR empt;
    FILE* dsk = fopen(path.c_str(), "rb+");
    EBR tempebr;
    Mounter Montado;
    fseek(dsk, startpoint, SEEK_SET);
    string s_a;
    fread(&tempebr, sizeof(tempebr), 1, dsk);
    int tam = nombre.length();
    s_a = compart.convertToString(tempebr.name, tam);

    if(s_a == nombre){

        Montado.id = "27"+ite+nombre;
        Montado.name = nombre;
        Montado.path = path;
        Montado.startpoint = tempebr.start;
        cout<<"El id de la particion es: " + Montado.id <<endl;
        return Montado;
    }else if(tempebr.p_siguiente != -1){
        return MontarEBR(dir, nombre, tempebr.p_siguiente, ite+1);
    }else{
        cout<< "No se pudo encontrar la particion que se deseaba montar"<<endl;
    }
    return Montado;

}

std::vector<Mounter> Mount::Unmount(string id, vector<Mounter> Montados){

    for (int i = 0; i < Montados.size(); i++){
        if(id == Montados[i].id){
            Montados.erase(Montados.begin()+i);
            return Montados;
        }
    }
    
    return Montados;
}