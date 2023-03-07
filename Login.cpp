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

class Login {
    public:

    credentials Loguear(vector<Mounter> Montados, Parametros parameters);
    vector<string> Separar(string Cadena);
    credentials ReturnUsser(vector<string> Datos);
    string replace_txt(string str, const string& from, const string& to);
};



credentials Login::Loguear(vector<Mounter> Montados, Parametros parameters){

    bool PMontada = false;
    string path;
    int startpoint = 0;
    char pass[64];
    char uss[64];
    credentials UsuarioInf;

    for (int i = 0; i < Montados.size(); i++){
        if(parameters.nombre == Montados[i].id){
            path = Montados[i].path;
            startpoint = Montados[i].startpoint;
            PMontada = true;
        }
    }


    if(!PMontada){
        cout<< "No se encontro la particion deseada"<< endl;
        return UsuarioInf;
    }

    FILE* dsk = fopen(path.c_str(), "rb+");
    fseek(dsk, startpoint, SEEK_SET);

    Sblock super_block;

    fread(&super_block, sizeof(Sblock), 1, dsk);

    BloqueArchivos bl_archivos;
    fseek(dsk, super_block.s_block_start + sizeof(BloqueCarpetas), SEEK_SET);
    fread(&bl_archivos, sizeof(BloqueArchivos), 1, dsk);
    
    string Bloquear = bl_archivos.b_content;
    vector<string> splited = Separar(Bloquear);

    UsuarioInf = ReturnUsser(splited);

    strcpy(pass, parameters.pass.c_str());
    strcpy(uss, parameters.user.c_str());

    if(UsuarioInf.usser != uss){
        cout<< "El usuario no pudo ser encontrado"<<endl;
        return UsuarioInf;
    }

    if(UsuarioInf.pass != pass){
        cout<< "No se pudo acceder a la cuenta deseada"<< endl;
        return UsuarioInf;
    }

    UsuarioInf.logueado = true;

    return UsuarioInf;

}

vector<string> Login::Separar(string Cadena){
    
    stringstream text_to_split(Cadena);
    string segment;
    vector<string> splited;
    
    while(getline(text_to_split, segment,'\n')){
       splited.push_back(segment);
    }
    return splited;
}

credentials Login::ReturnUsser(vector<string> Datos){

    credentials cosas;
    vector<string> splited;
    string segment;
    string actuall;

    for (int i = 0; i < Datos.size(); i++)
    {
        if(Datos[i].find("1,U,") == 0){
            actuall = replace_txt(Datos[i], "1,U,", "");
            stringstream text_to_split(actuall);
            
            while(getline(text_to_split, segment,',')){
            splited.push_back(segment);
            }
            cosas.pass = splited[2];
            cosas.usser = splited[1];
            return cosas;
        }
    }
    return cosas;
}

string Login::replace_txt(string str, const string& from, const string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}