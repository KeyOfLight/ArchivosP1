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
    Grupo ReturnGroup(vector<string> Datos);
    void CreateGrp(User Usuario, Parametros parameters);
    vector<string> Separar(string Cadena);
    string replace_txt(string str, const string& from, const string& to);
    void CreateNewFileBlock(I_node InodoArchivo, string Content, int BlockStartPoint, File* disk, int InodoStart);
    int BlockPosition(File* dsk, int BlockStartPoint);
}

void Mkgrp::CreateGrp(User Usuario, Parametros parameters){
    string path = Usuario.path;
    int startpoint = Usuario.startpoint;
    vector<Grupo> Grupos;

    if(Usuario.Uss != "root"){
        cout<< "Necesita tener accesos de admin B) para hacer esta modificacion"<< endl;
        return;
    }

    FILE* dsk = fopen(path.c_str(), "rb+");
    fseek(dsk, startpoint, SEEK_SET);


    //Leer Superblock
    Sblock super_block;
    fread(&super_block, sizeof(Sblock), 1, dsk);

    I_node inodoArchivo;
    fseek(dsk, SupBlock.s_inode_start + sizeof(I_node), SEEK_SET);
    fread(&inodoArchivo, sizeof(I_node), 1, dsk);
    

    BloqueArchivos bl_archivos;
    fseek(dsk, super_block.s_block_start + sizeof(BloqueCarpetas), SEEK_SET);
    fread(&bl_archivos, sizeof(BloqueArchivos), 1, dsk);

    string content = bl_archivos.b_content;

    vector<string> content2 = Separar(content);
    Grupos = ReturnGroup(content2);

    char novogrp[11];
    cout << "Ingrese el nombre del grupo que desea agregar"<<endl;
    cin >> novogrp;

    for (int i = 0; i < Grupos.size(); i++){
        int value = strcmp(Grupos[i].Grupo,novogrp);
        if(value == 0){
            cout << "Ya existe un grupo con este nombre"<<endl;
        }
    }

    content += Grupos[Grupos.size()-1].GID+",G"+novogrp+"\n";
    
    if(content.size()< 64){
        strcpy(bl_archivos.b_content, contenido.c_str());
        fseek(dsk, super_block.s_block_start + sizeof(BloqueCarpetas), SEEK_SET);
        fwrite(&bl_archivos, sizeof(BloqueArchivos), 1, dsk);
    }else{
        string Aux;
        string Aux2;
        for (int i = 0; i < 64; i++){
            Aux2 += content[i];

        }

        for (int i = 64; i < content.size(); i++){
            Aux += content[i];
        }
        
        CreateNewFileBlock(inodoArchivo, content, super_block.s_block_start, dsk, SupBlock.s_inode_start + sizeof(I_node));
    }


}




void Mkgrp::CreateNewFileBlock(I_node InodoArchivo, string Content, int BlockStartPoint, File* dsk, int InodoStart){

    int contador;

    //Comprobar si se pueden guardar los datos en un bloque ya creado
    for (int i = 0; i < 12; i++){
        if(InodoArchivo.i_block[i] != -1){
            contador = InodoArchivo.i_block[i];

            BloqueArchivos bl_archivo;
            strcpy(bl_archivo.b_content, Content.c_str());
            fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
            fread(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
            string ContenidoStr;
            ContenidoStr = bl_archivo.b_content;

            //Se comprueba que el largo del bloque de archivo sea menor a 64
            if(ContenidoStr.length() < 64){
                ContenidoStr += Content;
                if(ContenidoStr.length() < 64){
                    strcpy(bl_archivo.b_content, ContenidoStr.c_str());
                    fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
                    fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);

                    return;  
                }else{
                    string Aux;
                    string Aux2;
                    for (int i = 0; i < 64; i++){
                        Aux2 += ContenidoStr[i];
                    }

                    strcpy(bl_archivo.b_content, Aux2.c_str());
                    fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
                    fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);

                    for (int i = 64; i < ContenidoStr.length(); i++){
                        Aux += content[i];
                    }
                    CreateNewFileBlock(InodoArchivo, Aux, BlockStartPoint, dsk, InodoStart);
                }   
            }
        }
    }
    

    //Apuntador directo nuevo
    for (int i = 0; i < 12; i++){
        if(InodoArchivo.i_block[i] == -1){

            contador = BlockPosition(dsk, BlockStartPoint);
            InodoArchivo.i_block[i] = contador;

            fseek(dsk, InodoStart, SEEK_SET);
            fwrite(&InodoArchivo, sizeof(I_node), 1, dsk);

            BloqueArchivos bl_archivo;
            strcpy(bl_archivo.b_content, Content.c_str());
            fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
            fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
            return;
        }
    }

    //Apuntador indirecto
    if(InodoArchivo.i_block[12] == -1){
        BloqueApuntadores BPointer;
        contador = BlockPosition(dsk, BlockStartPoint);

        //Se modifica el indo del archivo ussers
        InodoArchivo.i_block[12] = contador;
        fseek(dsk, InodoStart, SEEK_SET);
        fwrite(&InodoArchivo, sizeof(I_node), 1, dsk);

        //Se escribe el primer pointer 
        BPointer[0] = contador + 1;
        fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*contador, SEEK_SET);
        fwrite(&BPointer, sizeof(BloqueApuntadores), 1, dsk);

        //Se escribe el contenido del archivo
        BloqueArchivos bl_archivo;
        strcpy(bl_archivo.b_content, Content.c_str());
        fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*(contador + 1), SEEK_SET);
        fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
        return;
    }else{
        contador = InodoArchivo.i_block[12];

        BloqueApuntadores BPointer;
        fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*contador, SEEK_SET);
        fread(&BPointer, sizeof(BloqueApuntadores), 1, dsk);
        //////No terminado

    }

    //Apuntador doble indirecto
    if(InodoArchivo.i_block[13] == -1){

        BloqueApuntadores BPointer;
        contador = BlockPosition(dsk, BlockStartPoint);

        //Se modifica el indo del archivo ussers
        InodoArchivo.i_block[12] = contador;
        fseek(dsk, InodoStart, SEEK_SET);
        fread(&InodoArchivo, sizeof(I_node), 1, dsk);

        //Se escribe el primer pointer 
        BPointer[0] = contador + 1;
        fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*contador, SEEK_SET);
        fwrite(&BPointer, sizeof(BloqueApuntadores), 1, dsk);

        BloqueApuntadores BPointer2;
        contador += 1;

        BPointer2.b_pointers[0] = contador +1;
        fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*contador, SEEK_SET);
        fwrite(&BPointer, sizeof(BloqueApuntadores), 1, dsk);

        BloqueArchivos bl_archivo;
        strcpy(bl_archivo.b_content, Content.c_str());
        fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*(contador + 1), SEEK_SET);
        fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
        return;
        
    }



}


int Mkgrp::BlockPosition(File* dsk, int BlockStartPoint){

    BloqueArchivos Contenido;
    int contador = 2;

    fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas), SEEK_SET);
    fread(&Contenido, sizeof(BloqueArchivos), 1, dsk);

    while (Contenido.b_content != NULL){
        fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
        fread(&Contenido, sizeof(BloqueArchivos), 1, dsk);
        contador += 1;
    }

    return contador;

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

Grupo Mkgrp::ReturnGroup(vector<string> Datos){

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