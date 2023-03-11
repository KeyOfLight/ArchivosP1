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

class Mkgrp{
public:
    vector<Grupo> ReturnGroup(vector<string> Datos);
    void CreateGrp(User Usuario, Parametros parameters);
    vector<string> Separar(string Cadena);
    string replace_txt(string str, const string& from, const string& to);

    Compartido Commons;
};

void Mkgrp::CreateGrp(User Usuario, Parametros parameters){
    string path = Usuario.path;
    int startpoint = Usuario.startpoint;
    vector<Grupo> Grupos;
    string Datos;

    int value = strcmp(Usuario.Uss,"root");
    if(value != 0){
        cout<< "Necesita tener accesos de admin B) para hacer esta modificacion"<< endl;
        return;
    }
    string ruta = "users.txt";

    FILE* dsk = fopen(path.c_str(), "rb+");
    Datos = Commons.LeerArchivoMkfs(dsk,startpoint,ruta);

    vector<string> content2 = Separar(Datos);
    Grupos = ReturnGroup(content2);

    char novogrp[11];
    cout << "Ingrese el nombre del grupo que desea agregar"<<endl;
    cin >> novogrp;

    int grpsize = Grupos.size();

    for (int i = 0; i < grpsize; i++){
        int value = strcmp(Grupos[i].Grupo,novogrp);
        if(value == 0){
            cout << "Ya existe un grupo con este nombre"<<endl;
        }
    }
    string content;
    string novogrupo = novogrp;
    int GrpNum = atoi(&Grupos[grpsize-1].GID) + 1;
    content += to_string(GrpNum)+",G,"+novogrupo+"\n";

    bool resul;
    resul = Commons.WriteFileBlock(dsk, startpoint, "users.txt", content);
}

vector<string> Mkgrp::Separar(string Cadena){
    
    stringstream text_to_split(Cadena);
    string segment;
    vector<string> splited;
    
    while(getline(text_to_split, segment,'\n')){
       splited.push_back(segment);
    }
    return splited;
}

vector<Grupo> Mkgrp::ReturnGroup(vector<string> Datos){

    Grupo cosas;
    vector<string> splited;
    string segment;
    string actuall;
    vector<Grupo> Retornar;

    for (int i = 0; i < Datos.size(); i++){
        if(Datos[i].find("1,G,") == 0){
            actuall = replace_txt(Datos[i], "1,G,", "1,G,");
            stringstream text_to_split(actuall);
            
            while(getline(text_to_split, segment,',')){
            splited.push_back(segment);
            }
            strcpy(&cosas.GID, splited[0].c_str());
            strcpy(&cosas.Type, splited[1].c_str());
            strcpy(cosas.Grupo, splited[2].c_str());
            
            Retornar.push_back(cosas);
        }
    }
    return Retornar;
}

string Mkgrp::replace_txt(string str, const string& from, const string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}