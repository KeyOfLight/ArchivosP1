#pragma once
#include <iostream>
#include "Estructuras.cpp"
#include <bits/stdc++.h>
#include <fstream>
#include <vector>

using namespace std;

class Compartido{
public:
    int VerFit(string Actualfit);
    int realsize(int tam, string unidad);
    string convertToString(char* a, int size);
    string LeerArchivoMkfs(FILE* dsk, int startpoint, string pathMkfs);

    string ReadBlckCarpetas(int pos, FILE* dsk, vector<string> coincidencia, Sblock SupBlock);
    string ReadInodo(int pos, FILE* dsk, vector<string> coincidencia, Sblock SupBlock);

    string ReadFileBlocks(I_node InodoArchivo, int BlockStartPoint, FILE* dsk, int InodoStart);
    string ReadPointer(BloqueApuntadores Apuntador, FILE* dsk, int BlockStartPoint);

    bool WrteBlckCarpetas(int pos, FILE* dsk, vector<string> coincidencia, Sblock SupBlock, string Escribir);
    bool WriteFileBlock(FILE* dsk, int startpoint, string pathMkfs, string Escribir);
    bool CreateNewFileBlock(I_node InodoArchivo, string Content, FILE* dsk, Sblock Superblock, int posInodo);
    int BlockPosition(FILE* dsk, Sblock SuperBlock);
    bool CheckPointer(BloqueApuntadores Apuntador, FILE* dsk, string Content, Sblock Superblock, int PosPointer);
    bool WrtInodo(int pos, FILE* dsk, vector<string> coincidencia, Sblock SupBlock, string Escribir);

    vector<string> split_Path(string text);
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


string Compartido::convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////ESCRIBIR ARCHIVOS
bool Compartido::WriteFileBlock(FILE* dsk, int startpoint, string pathMkfs, string Escribir){

    vector<string> PathSeparado;
    int pos;
    int value;
    string DatosArch = "";
    fseek(dsk, startpoint, SEEK_SET);
    //Leer Superblock
    Sblock super_block;
    fread(&super_block, sizeof(Sblock), 1, dsk);

    I_node InodoRoot;
    fseek(dsk, super_block.s_inode_start, SEEK_SET);
    fread(&InodoRoot, sizeof(I_node), 1, dsk);

    //Separar Path por /
    PathSeparado = split_Path(pathMkfs);

    for (int i = 0; i < 15; i++){
        if(InodoRoot.i_block[i] != -1){
            pos = InodoRoot.i_block[i];
            if(WrteBlckCarpetas(pos,dsk,PathSeparado, super_block, Escribir)){
                return true;
            }
        }
    }
    return false;
};

bool Compartido::WrteBlckCarpetas(int pos, FILE* dsk, vector<string> coincidencia, Sblock SupBlock, string Escribir){

    BloqueCarpetas bloqueCarpeta;
    int value;
    int SearchPoint = SupBlock.s_block_start + sizeof(BloqueCarpetas) * pos;

    fseek(dsk, SearchPoint, SEEK_SET);
    fread(&bloqueCarpeta, sizeof(BloqueCarpetas), 1, dsk);

    int largo = sizeof(bloqueCarpeta.b_content) / sizeof(bloqueCarpeta.b_content[0]);
    for (int i = 0; i < largo; i++){
        Content Actual;
        Actual = bloqueCarpeta.b_content[i];
        int size_path = size(coincidencia[0]);
        char PathAct[size_path];
        strcpy(PathAct, coincidencia[0].c_str());

        value = strcmp(Actual.b_name,PathAct);  
        if(value == 0){
            pos = Actual.b_inodo;
            coincidencia.erase(coincidencia.begin());
            if(WrtInodo(pos, dsk, coincidencia, SupBlock, Escribir)){
                return true;
            }
        }
    }
    return false;
};

bool Compartido::WrtInodo(int pos, FILE* dsk, vector<string> coincidencia, Sblock SupBlock, string Escribir){

    I_node inodoArchivo;
    char *Archivo;
    int value;
    fseek(dsk, SupBlock.s_inode_start + sizeof(I_node) * pos, SEEK_SET);
    fread(&inodoArchivo, sizeof(I_node), 1, dsk);

    strcpy(Archivo, "1");

    value = strcmp(&inodoArchivo.i_type, Archivo);  

    if(value == 0){
        if(CreateNewFileBlock(inodoArchivo, Escribir, dsk, SupBlock, SupBlock.s_inode_start + sizeof(I_node) * pos)){
            return true;
        }
    }
    //FALTA BUSCAR PARA CARPETAS WIP

    return false;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool Compartido::CreateNewFileBlock(I_node InodoArchivo, string Content, FILE* dsk, Sblock Superblock, int posInodo){

    int contador;
    int BlockStartPoint = Superblock.s_block_start;
    int InodoStart = posInodo;

    //Comprobar si se pueden guardar los datos en un bloque ya creado
    for (int i = 0; i < 12; i++){
        if(InodoArchivo.i_block[i] != -1){
            contador = InodoArchivo.i_block[i];
            int pos = BlockStartPoint + sizeof(BloqueCarpetas)*contador;

            BloqueArchivos bl_archivo;
            fseek(dsk, pos, SEEK_SET);
            fread(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
            string ContenidoStr;
            ContenidoStr = bl_archivo.b_content;

            //Se comprueba que el largo del bloque de archivo sea menor a 64
            if(ContenidoStr.length() < 63){
                ContenidoStr += Content;
                if(ContenidoStr.length() < 63){
                    strcpy(bl_archivo.b_content, ContenidoStr.c_str());
                    fseek(dsk, pos, SEEK_SET);
                    fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
                    cout<< bl_archivo.b_content <<endl;
                    return true;  
                }else{
                    
                    string Aux = ContenidoStr.substr(63);
                    string Aux2 = ContenidoStr.substr(0, 63);

                    strcpy(bl_archivo.b_content, Aux2.c_str());
                    fseek(dsk, pos, SEEK_SET);
                    fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);

                    if(CreateNewFileBlock(InodoArchivo, Aux, dsk, Superblock, posInodo)){
                        return true;
                    }
                }
            }
        }else{
            break;
        }
    }

    //Apuntador directo nuevo
    for (int i = 0; i < 12; i++){
        if(InodoArchivo.i_block[i] == -1){

            BloqueArchivos bl_archivo;

            contador = BlockPosition(dsk, Superblock);
            InodoArchivo.i_block[i] = contador;

            fseek(dsk, InodoStart, SEEK_SET);
            fwrite(&InodoArchivo, sizeof(I_node), 1, dsk);

            if(Content.length() < 63){

                strcpy(bl_archivo.b_content, Content.c_str());
                fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
                fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
                return true;
            }else{
                

                string Aux = Content.substr(63);
                string Aux2 = Content.substr(0, 63);
                strcpy(bl_archivo.b_content, Aux2.c_str());
                fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
                fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);

                if(CreateNewFileBlock(InodoArchivo, Aux, dsk, Superblock, posInodo)){
                    return true;
                }
            }
        }
    }

    //Si el apuntador indirecto ya tiene informacion
    if(InodoArchivo.i_block[12] != -1){
        BloqueApuntadores BPointer;
        contador = InodoArchivo.i_block[12];

        //Se busca el bloque de apuntador
        fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*contador, SEEK_SET);
        fread(&BPointer, sizeof(BloqueApuntadores), 1, dsk);

        if(CheckPointer(BPointer, dsk, Content, Superblock, BlockStartPoint + sizeof(BloqueApuntadores)*contador)){
            return true;
        }

    }else if(InodoArchivo.i_block[12] == -1){//Apuntador indirecto nuevo
        BloqueApuntadores BPointer;
        contador = BlockPosition(dsk, Superblock);

        //Se modifica el indo del archivo ussers
        InodoArchivo.i_block[12] = contador;
        fseek(dsk, InodoStart, SEEK_SET);
        fwrite(&InodoArchivo, sizeof(I_node), 1, dsk);

        contador = BlockPosition(dsk, Superblock);
        //Se escribe el primer pointer 
        BPointer.b_pointers[0] = contador;

        for (int i = 1; i < 16; i++){
            BPointer.b_pointers[i] = -1;
        }
        int bsp = BlockStartPoint + sizeof(BloqueApuntadores)*InodoArchivo.i_block[12];
        fseek(dsk, bsp, SEEK_SET);
        fwrite(&BPointer, sizeof(BloqueApuntadores), 1, dsk);

        //Se escribe el contenido del archivo
        BloqueArchivos bl_archivo;

        if(Content.length() < 63){

            strcpy(bl_archivo.b_content, Content.c_str());
            fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*BPointer.b_pointers[0], SEEK_SET);
            fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);

            return true;
        }else{
            string Aux = Content.substr(63);
            string Aux2 = Content.substr(0, 63);
            strcpy(bl_archivo.b_content, Aux2.c_str());
            fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*BPointer.b_pointers[0], SEEK_SET);
            fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);

            if(CreateNewFileBlock(InodoArchivo, Aux, dsk, Superblock, posInodo)){
                return true;
            }
        }
    }

    //Apuntador indirecto doble con informacion
    if(InodoArchivo.i_block[13] != -1){
        BloqueApuntadores BPointer;
        BloqueApuntadores BPointer2;
        contador = InodoArchivo.i_block[12];
        int contador2;
        bool Escrito = false;

        //Se busca el bloque de apuntador
        fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*contador, SEEK_SET);
        fread(&BPointer, sizeof(BloqueApuntadores), 1, dsk);

        //Se Recorre el primer bloque de aputnadores
        for (int j = 0; j < 16; j++){
            //Se verifica que el apuntador ya este apuntando a otro apuntador
            if(BPointer.b_pointers[j] != -1){
                contador2 = BPointer.b_pointers[j];
                fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*contador2, SEEK_SET);
                fread(&BPointer2, sizeof(BloqueApuntadores), 1, dsk);

                Escrito = CheckPointer(BPointer2, dsk, Content, Superblock, BlockStartPoint + sizeof(BloqueApuntadores)*contador2);

                if(Escrito)
                    return true;

            }else if(BPointer.b_pointers[j] == -1){//Se crea un nuevo apuntador por si este no apunta a ningun apuntador
                contador2 = BlockPosition(dsk, Superblock);
                BloqueApuntadores BPointer2;

                for (int i = 0; i < 16; i++){
                    BPointer2.b_pointers[i] = -1;
                }
                fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*contador2, SEEK_SET);
                fwrite(&BPointer2, sizeof(BloqueApuntadores), 1, dsk);
                Escrito = CheckPointer(BPointer, dsk, Content, Superblock, BlockStartPoint + sizeof(BloqueApuntadores)*contador2);

                if(Escrito)
                    return true;
            }
        }
    }else if(InodoArchivo.i_block[13] == -1){ //Apuntador doble indirecto nuevo

        BloqueApuntadores BPointer;
        contador = BlockPosition(dsk, Superblock);

        //Se modifica el indo
        InodoArchivo.i_block[12] = contador;
        fseek(dsk, InodoStart, SEEK_SET);
        fread(&InodoArchivo, sizeof(I_node), 1, dsk);

        //Se escribe el primer pointer 
        contador = BlockPosition(dsk, Superblock);
        BPointer.b_pointers[0] = contador;
        for (int i = 1; i < 16; i++){
            BPointer.b_pointers[i] = -1;
        }
        fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*InodoArchivo.i_block[12], SEEK_SET);
        fwrite(&BPointer, sizeof(BloqueApuntadores), 1, dsk);

        BloqueApuntadores BPointer2;
        contador = BlockPosition(dsk, Superblock);
        BPointer2.b_pointers[0] = contador;
        for (int i = 1; i < 16; i++){
            BPointer2.b_pointers[i] = -1;
        }
        fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*BPointer.b_pointers[0], SEEK_SET);
        fwrite(&BPointer2, sizeof(BloqueApuntadores), 1, dsk);

        BloqueArchivos bl_archivo;

        if(Content.length() < 63){
            strcpy(bl_archivo.b_content, Content.c_str());
            fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*(BPointer2.b_pointers[0]), SEEK_SET);
            fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
            return true;
        }else{
            string Aux = Content.substr(63);
            string Aux2 = Content.substr(0, 63);
            strcpy(bl_archivo.b_content, Aux2.c_str());
            fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*(BPointer2.b_pointers[0]), SEEK_SET);
            fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);

            if(CreateNewFileBlock(InodoArchivo, Aux, dsk, Superblock, posInodo)){
                return true;
            }
        }
        
    }
    return false;
}
// Revisa que los apuntadores tengan inforamcion, de lo contrario crea un bloque de archivos para agregarle informacion
bool Compartido::CheckPointer(BloqueApuntadores Apuntador, FILE* dsk, string Content, Sblock SuperBlock, int PosPointer){
    int contador;
    int BlockStartPoint = SuperBlock.s_block_start;

    for (int i = 0; i < 16; i++){
        if(Apuntador.b_pointers[i] != -1){
            contador = Apuntador.b_pointers[i];

            BloqueArchivos bl_archivo;
            fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
            fread(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
            string ContenidoStr;
            ContenidoStr = bl_archivo.b_content;


                //Se comprueba que el largo del bloque de archivo sea menor a 64
            if(ContenidoStr.length() < 63){
                ContenidoStr += Content;
                if(ContenidoStr.length() < 63){
                    strcpy(bl_archivo.b_content, ContenidoStr.c_str());
                    fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
                    fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
                    return true;  
                }else{

                    string Aux = ContenidoStr.substr(63);
                    string Aux2 = ContenidoStr.substr(0, 63);
                    strcpy(bl_archivo.b_content, Aux2.c_str());
                    fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
                    fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
                    if(CheckPointer(Apuntador, dsk, Aux, SuperBlock, PosPointer)){
                        return true;
                    }
                }
            }
            //Revisa si el pointer esta vacio
        }else if(Apuntador.b_pointers[i] == -1){
            contador = BlockPosition(dsk, SuperBlock);
            Apuntador.b_pointers[i] = contador;
            fseek(dsk, PosPointer, SEEK_SET);
            fwrite(&Apuntador, sizeof(BloqueApuntadores), 1, dsk);

            BloqueArchivos bl_archivo;

            if(Content.length() < 63){
                strcpy(bl_archivo.b_content, Content.c_str());
                fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
                fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
                return true;  

            }else{
            
                string Aux = Content.substr(63);
                string Aux2 = Content.substr(0, 63);

                strcpy(bl_archivo.b_content, Aux2.c_str());
                fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
                fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);

                if(CheckPointer(Apuntador, dsk, Aux, SuperBlock, PosPointer)){
                    return true;
                }

            }
        }
    }
    return false;
}

//Bitmap block hacer
int Compartido::BlockPosition(FILE* dsk, Sblock Superblock){

    char Bloque;
    int contador = 0;
    
    int BMB = Superblock.s_bm_block_start;
    fseek(dsk, BMB, SEEK_SET);
    fread(&Bloque, sizeof(char), 1, dsk);

    while (Bloque != '0'){
        contador += 1;
        fseek(dsk, BMB + contador, SEEK_SET);
        fread(&Bloque, sizeof(char), 1, dsk);
    }
    Bloque = '1';
    fseek(dsk, BMB + contador, SEEK_SET);
    fwrite(&Bloque, sizeof(char), 1, dsk);

    return contador;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////Leer Archivos
string Compartido::LeerArchivoMkfs(FILE* dsk, int startpoint, string pathMkfs){

    vector<string> PathSeparado;
    int pos;
    int value;
    string DatosArch = "";
    fseek(dsk, startpoint, SEEK_SET);
    //Leer Superblock
    Sblock super_block;
    fread(&super_block, sizeof(Sblock), 1, dsk);

    I_node InodoRoot;
    fseek(dsk, super_block.s_inode_start, SEEK_SET);
    fread(&InodoRoot, sizeof(I_node), 1, dsk);

    //Separar Path por /
    PathSeparado = split_Path(pathMkfs);

    for (int i = 0; i < 15; i++){
        if(InodoRoot.i_block[i] != -1){
            pos = InodoRoot.i_block[i];
            DatosArch += ReadBlckCarpetas(pos,dsk,PathSeparado, super_block);
            return DatosArch;
        }
    }
    return DatosArch;
};

string Compartido::ReadInodo(int pos, FILE* dsk, vector<string> coincidencia, Sblock SupBlock){

    I_node inodoArchivo;
    string Datos = "";
    char *Archivo;
    int value;
    fseek(dsk, SupBlock.s_inode_start + sizeof(I_node) * pos, SEEK_SET);
    fread(&inodoArchivo, sizeof(I_node), 1, dsk);

    strcpy(Archivo, "1");

    value = strcmp(&inodoArchivo.i_type, Archivo);  

    if(value == 0){
        Datos += ReadFileBlocks(inodoArchivo, SupBlock.s_block_start, dsk, SupBlock.s_inode_start);
    }

    return Datos;

};

string Compartido::ReadBlckCarpetas(int pos, FILE* dsk, vector<string> coincidencia, Sblock SupBlock){

    BloqueCarpetas bloqueCarpeta;
    int value;
    string Datos = "";
    int SearchPoint = SupBlock.s_block_start + sizeof(BloqueCarpetas) * pos;

    fseek(dsk, SearchPoint, SEEK_SET);
    fread(&bloqueCarpeta, sizeof(BloqueCarpetas), 1, dsk);

    int largo = sizeof(bloqueCarpeta.b_content) / sizeof(bloqueCarpeta.b_content[0]);
    for (int i = 0; i < largo; i++){
        Content Actual;
        Actual = bloqueCarpeta.b_content[i];
        int size_path = size(coincidencia[0]);
        char PathAct[size_path];
        strcpy(PathAct, coincidencia[0].c_str());

        value = strcmp(Actual.b_name,PathAct);  
        if(value == 0){
            pos = Actual.b_inodo;
            coincidencia.erase(coincidencia.begin());
            Datos += ReadInodo(pos, dsk, coincidencia, SupBlock);
            return Datos;
        }
    }
    return Datos;
};

string Compartido::ReadFileBlocks(I_node InodoArchivo, int BlockStartPoint, FILE* dsk, int InodoStart){

    int contador;
    string ContenidoStr = "";

    //Leer el contenido de un bloque de archivos
    for (int i = 0; i < 12; i++){
        if(InodoArchivo.i_block[i] != -1){
            contador = InodoArchivo.i_block[i];

            BloqueArchivos bl_archivo;
            fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
            fread(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
            ContenidoStr += bl_archivo.b_content;
        }
    }

    //Si el apuntador indirecto ya tiene informacion la lee
    if(InodoArchivo.i_block[12] != -1){
        BloqueApuntadores BPointer;
        contador = InodoArchivo.i_block[12];

        //Se busca el bloque de apuntador
        fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*contador, SEEK_SET);
        fread(&BPointer, sizeof(BloqueApuntadores), 1, dsk);

        ContenidoStr += ReadPointer(BPointer, dsk, BlockStartPoint);
    }

    //Apuntador indirecto doble con informacion
    if(InodoArchivo.i_block[13] != -1){
        BloqueApuntadores BPointer;
        BloqueApuntadores BPointer2;
        contador = InodoArchivo.i_block[12];
        int contador2;
        bool Escrito = false;

        //Se busca el bloque de apuntador
        fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*contador, SEEK_SET);
        fread(&BPointer, sizeof(BloqueApuntadores), 1, dsk);

        //Se Recorre el primer bloque de aputnadores
        for (int j = 0; j < 16; j++){
            //Se verifica que el apuntador ya este apuntando a otro apuntador
            if(BPointer.b_pointers[j] != -1){
                contador2 = BPointer.b_pointers[j];
                fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*contador2, SEEK_SET);
                fread(&BPointer2, sizeof(BloqueApuntadores), 1, dsk);

                ContenidoStr += ReadPointer(BPointer, dsk, BlockStartPoint);

            }
        }
    }

    return ContenidoStr;
};

string Compartido::ReadPointer(BloqueApuntadores Apuntador, FILE* dsk, int BlockStartPoint){
    int contador;
    string ContenidoStr;
    ContenidoStr = "";

    for (int i = 0; i < 16; i++){
        if(Apuntador.b_pointers[i] != -1){
            contador = Apuntador.b_pointers[i];

            BloqueArchivos bl_archivo;
            fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
            fread(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
            ContenidoStr += bl_archivo.b_content;
            
        }
    }
    return ContenidoStr;
};


vector<string> Compartido::split_Path(string text){ // Split para separar el path basado en /
    stringstream text_to_split(text);
    string segment;
    vector<string> splited;
    
    while(getline(text_to_split, segment,'/')){
       splited.push_back(segment);
    }
    return splited;

};