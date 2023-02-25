#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Estructuras.cpp"
#include <stdio.h>
#include <string.h>
#include <bits/stdc++.h>
#include "Compartido.cpp"

using namespace std;

class Comandos{
public:
    void MkDisk(Parametros parameters);
    bool existeDisco(string path);
    vector<string> PathCleanner(string fullpath);
    void customSplit(string str, char separator);
    int realsize(int tam, string unidad);


    Comando cmd;


    vector < string > strings;

};

bool Comandos::existeDisco(string path) {
    FILE *disk = fopen(path.c_str(), "rb");

    cout << path << endl;

    if (!disk) {
        return false;
    }

    fclose(disk);
    return true;
}

void Comandos::MkDisk(Parametros parameters){
    string fullpath = parameters.direccion; //Direccion completa de donde se debe crear el archivo

    if (existeDisco(fullpath)) {
        cout << endl << " *** El disco ya existe ***" << endl << endl;
        return;
    }


    time_t t;
    struct tm *tm;
    char fechayhora[20];

    t = time(NULL);
    tm = localtime(&t);
    strftime(fechayhora, 20, "%d/%m/%Y %H:%M:%S", tm);

    int size = stoi(parameters.tam);
    // Crear MBR del disco
    MBR mbr;
    size = realsize(size,parameters.unit);
    mbr.tamano = size;
    strcpy(mbr.fecha_creacion, fechayhora);
    mbr.disk_signature = rand() % 1000;

    Compartido compa;
    int currentFit;

    currentFit = compa.VerFit(parameters.fit);

    if(currentFit == 0){//Best Fit
        mbr.disk_fit = 'b';
    }else if(currentFit == 1){//First Fit
        mbr.disk_fit = 'f';
    }else if(currentFit == 2){//Worst Fit
        mbr.disk_fit = 'w';
    }


    cout << endl << "Disco creado" << endl;
    cout << "Size: " << mbr.tamano << parameters.unit << endl;
    cout << "Fecha: " << mbr.fecha_creacion << endl;
    cout << "Signature: " << mbr.disk_signature  << endl;

    
    // Crear el disco y escribir el mbr dentro
    FILE* disco = fopen(fullpath.c_str(), "wb+");
    if (disco != NULL) {
        fwrite("\0", 1, 1, disco);
        fseek(disco, size - 1, SEEK_SET);
        fwrite("\0", 1, 1, disco);
        rewind(disco);
        fwrite(&mbr, sizeof(MBR), 1, disco);

        cout << endl << "Disco Creado Exitosamente" << endl << endl;
    } else {
        vector<string> limpio;
        limpio = PathCleanner(fullpath);
        string path = limpio.back();
        limpio.pop_back();
        // Crear la estructura de directorios
        string comando1 = "sudo mkdir -p \"" + path + "\"";
        string comando2 = "sudo chmod -R 777 \"" + path + "\"";
        system(comando1.c_str());
        system(comando2.c_str());

        disco = fopen(fullpath.c_str(), "wb+");
        fwrite("\0", 1, 1, disco);
        fseek(disco, size - 1, SEEK_SET);
        fwrite("\0", 1, 1, disco);
        rewind(disco);
        fwrite(&mbr, sizeof(MBR), 1, disco);

        cout << endl << "Disco Creado Exitosamente" << endl << endl;
    }

    fclose(disco);

    return;


}

void Comandos::customSplit(string str, char separator) {
    int startIndex = 0, endIndex = 0;
    for (int i = 0; i <= str.size(); i++) {
        
        // If we reached the end of the word or the end of the input.
        if (str[i] == separator || i == str.size()) {
            endIndex = i;
            string temp;
            temp.append(str, startIndex, endIndex - startIndex);
            strings.push_back(temp);
            startIndex = endIndex + 1;
        }
    }
}

int Comandos::realsize(int tam, string unidad){

    int newsize;
    if(unidad.find("k") == 0){
        newsize = tam * 1024;
    }else if(unidad.find("m") == 0){
        newsize = tam * 1024 * 1024;
    }else {
        newsize = tam * 1024 * 1024;
    }

    return newsize;
}

vector<string> Comandos::PathCleanner(string fullpath){//Separa el path completo del nombre del archivo

    if(!fullpath.find(".dsk")){
        stringstream ss(fullpath);
    }
    vector < string > completo;

    string path = "";

    char separator = '/'; 
    customSplit(fullpath, separator);


    completo.push_back(strings.back());
    strings.pop_back();

    for (auto it: strings) {
        path += it + '/';
    }
    path.pop_back();
    completo.push_back(path);

    return completo;
}