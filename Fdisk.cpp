#include <iostream>
#include "Estructuras.cpp"
#include <bits/stdc++.h>
#include "Compartido.cpp"
#include <fstream>
#include <vector>
using namespace std;

class Fdisk
{
public:
    void CrearParticion(Parametros parameters);
    bool existeDisco(string path);
    int tamparticiones(MBR TempMbr);
    Particion ActivarParticion(Particion oldPart, Parametros parameters, int size, int startpoint);
    void EliminarParticion(Parametros parameters);
    string convertToString(char* a, int size);
    
    int GetPositionFF(MBR mbrActual, int Tam);
    MBR OrdenarArray(MBR AllArrays);
    bool IstheDiskEmpty(MBR mbrActual);
    int GetPositionBF(MBR mbrActual, int Tam);
    int CalcularSizeExt(int expart, Parametros parameters, int lpartsize);
    void AddSpace(Parametros parameters);

    EBR ActivarEBR(Parametros parameters, int size, int startpoint, int siguiente);
    EBR GetPositionFFEbr(Particion PartExt, int Tam,Parametros parameters);
    EBR GetPositionBFEbr(Particion PartExt, int Tam,Parametros parameters);
    void EliminarPartL(int startpoint, bool Completo, string name, string path);
    void AddLogic(int startpoint, int maxsize, string name, string path);

    int SearchNameEBR(Particion PartExt, string Nombre, Parametros parameters);

    Comando cmd;
    Compartido compart;
};

bool Fdisk::existeDisco(string path){

    char temp = path.back();

    if (temp == ' ')
        path.pop_back();

    FILE *disk = fopen(path.c_str(), "rb");

    if (!disk)
    {
        return false;
    }

    fclose(disk);
    return true;
}

void Fdisk::EliminarParticion(Parametros parameters){

    string path = parameters.direccion;
    Particion empt;
    MBR mbrActual;
    FILE* dsk = fopen(path.c_str(), "rb+");
    rewind(dsk);
    fread(&mbrActual, sizeof(mbrActual), 1, dsk);
    int extstart = 0;

    bool Discovacio = IstheDiskEmpty(mbrActual);
    
    if (Discovacio){
        cout<< "No se pudo encontrar la particion deseada"<< endl;
        return;
    }

    string s_a;

    for (int i = 0; i < 4; i++){
        if(mbrActual.particiones[i].status =='1'){
            int tam = parameters.nombre.length();
            s_a = convertToString(mbrActual.particiones[i].name, tam);
            if(mbrActual.particiones[i].type == 'e'){
                extstart = mbrActual.particiones[i].start;
            }
            if(s_a == parameters.nombre){
                string opcion;
                cout << "La particion fue encontrada, desea eliminarla? (S/N)"<<endl;
                cin >> opcion;
                if(opcion == "S" or opcion == "s"){
                        if(mbrActual.particiones[i].type == 'e'){
                            EliminarPartL(mbrActual.particiones[i].start, true, parameters.nombre, path);
                        }
                        mbrActual.particiones[i] = empt;
                        rewind(dsk);
                        fwrite(&mbrActual, sizeof(MBR), 1, dsk);
                        fclose(dsk);
                        return;
                }
            }
        }
    }

    if(extstart != 0){
        EliminarPartL(extstart, false, parameters.nombre, path);
        return;
    }

    cout<<"No se pudo encontrar la particion deseada"<<endl;
    return;

}

void Fdisk::EliminarPartL(int startpoint, bool Completo, string name, string dir){

    string path = dir;
    EBR empt;
    FILE* dsk = fopen(path.c_str(), "rb+");
    EBR tempebr;
    fseek(dsk, startpoint, SEEK_SET);
    string s_a;

    fread(&tempebr, sizeof(tempebr), 1, dsk);

    if(Completo){
        if(tempebr.status == '1'){  
            int next = tempebr.p_siguiente;
            fwrite(&empt, sizeof(EBR), 1, dsk);
            fclose(dsk);
            if(tempebr.p_siguiente != -1)
                EliminarPartL(next, Completo, name, dir);
        }

    }else{
        int tam = name.length();
        s_a = convertToString(tempebr.name, tam);
        if(s_a == name){
            string opcion;
            cout << "La particion fue encontrada, desea eliminarla? (S/N)"<<endl;
            cin >> opcion;
            if(opcion == "S" or opcion == "s"){
                    tempebr = empt;
                    rewind(dsk);
                    fwrite(&tempebr, sizeof(EBR), 1, dsk);
                    fclose(dsk);
                    return;
            }
        }else if(tempebr.p_siguiente != -1){
            EliminarPartL(tempebr.p_siguiente, Completo, name, dir);
            return;
        }else{
            cout<< "No se pudo encontrar la particion deseada"<<endl;
        }
    }

    return;
}

string Fdisk::convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}
 

void Fdisk::CrearParticion(Parametros parameters)
{ // Crea las particiones para los discos
    if(parameters.del == "full"){
        EliminarParticion(parameters);
        return;
    }

    if(stoi(parameters.add) > 0){
        AddSpace(parameters);
        return;
    }


    int size_ = stoi(parameters.tam);
    string unidad = parameters.unit;
    int newsize;
    int currentFit;
    int startpoint;

    string path = parameters.direccion;

    char temp = path.back();

    if (temp == ' ')
        path.pop_back();

    if (unidad.find("k") == 0)
    {
        newsize = size_ * 1024;
    }
    else if (unidad.find("m") == 0)
    {
        newsize = size_ * 1024 * 1024;
    }
    else
    {
        newsize = size_ * 1024 * 1024;
    }

    if (!existeDisco(path))
    {
        cout << "El disco al cual desea crearle una particion no existe o no pudo ser encontrado" << endl;
        return;
    }

    MBR mbr;

    FILE* dsk = fopen(path.c_str(), "rb+");

    if (dsk != NULL) {
        rewind(dsk);
        fread(&mbr, sizeof(mbr), 1, dsk);
    } else {
        cout << endl << " *** Error el path del disco no existe *** " << endl << endl ;
    }


    if (newsize < 0 and newsize > mbr.tamano)
    {
        cout << "Se debe de crear una particion que ocupe mas de 0 bytes o que se menor que el disco" << endl;
        fclose(dsk);
        return;
    }

    currentFit = compart.VerFit(parameters.fit);

    if(parameters.type == "p" or parameters.type == "" or parameters.type == "e"){

        if(currentFit == 0){//Best Fit
        startpoint = GetPositionBF(mbr, newsize);
        parameters.fit = "b";

        }else if(currentFit == 1){//First Fit
            startpoint = GetPositionFF(mbr, newsize);
            parameters.fit = "f";

        }else if(currentFit == 2){//Worst Fit
            

        }

    }

    if (parameters.type == "p" or parameters.type == ""){
        parameters.type = "p";
        int filledsize = tamparticiones(mbr); /// Obtiene todo el tama;o ocupado por las particiones ya existentes
        if (newsize < (mbr.tamano - filledsize)){
            Particion EmptPart;
            for (int i = 0; i < 4; i++){
                char* char_array = new char[16];
                strcpy(char_array, parameters.nombre.c_str());
                int result1 = strcmp(mbr.particiones[i].name, char_array);
                if(result1 == 0){
                    cout << "No se puede crear una particion con un nombre ya utilizado" << endl;
                    return;
                }
            }

            for (int i = 0; i < 4; i++){
                int result = strcmp(mbr.particiones[i].name, EmptPart.name);
                if (result == 0){
                    mbr.particiones[i] = ActivarParticion(mbr.particiones[i], parameters, newsize, startpoint);
                    rewind(dsk);
                    fwrite(&mbr, sizeof(MBR), 1, dsk);
                    break;
                }
            }
        }else {
            cout << "La particion debe ser menor al tam total del disco" << endl;
        }
    } else if(parameters.type == "e"){
        int filledsize = tamparticiones(mbr); /// Obtiene todo el tama;o ocupado por las particiones ya existentes
        if (newsize < (mbr.tamano - filledsize)){
            Particion EmptPart;
            for (int i = 0; i < 4; i++){
                char* char_array = new char[16];
                strcpy(char_array, parameters.nombre.c_str());
                int result1 = strcmp(mbr.particiones[i].name, char_array);
                if(result1 == 0){
                    cout << "No se puede crear una particion con un nombre ya utilizado" << endl;
                    return;
                }
                char newtype = 'e';
                int result2 = strcmp(&mbr.particiones[i].type, &newtype);
                if (result2 == 0){
                    cout << "Solo se puede tener una particion de tipo extendida"<< endl;
                    return;
                }
            }

            for (int i = 0; i < 4; i++){
                int result = strcmp(mbr.particiones[i].name, EmptPart.name);
                if (result == 0){
                    mbr.particiones[i] = ActivarParticion(mbr.particiones[i], parameters, newsize, startpoint);
                    rewind(dsk);
                    fwrite(&mbr, sizeof(MBR), 1, dsk);

                    break;
                }
            }
        }
    }  else if(parameters.type == "l"){
        EBR EmptEbr;
        Particion TempExt;

        //tTerminar
        for (int i = 0; i < 4; i++){
            char newtype = 'e';
            char typepart = mbr.particiones[i].type;
            if (newtype == typepart){
                TempExt = mbr.particiones[i];
            }
            char* char_array = new char[16];
            strcpy(char_array, parameters.nombre.c_str());
            int result1 = strcmp(mbr.particiones[i].name, char_array);
            if(result1 == 0){
                cout << "No se puede crear una particion con un nombre ya utilizado" << endl;
                return;
            }
        }

        //Verificar si existe una particion de tipo extendida
        if(TempExt.status != '1'){
            cout << "Para crear una particion logica es necesario que exista una particion extendida"<< endl;
            return;
        }

        if(SearchNameEBR(TempExt, parameters.nombre, parameters)){
            cout << "No se puede crear una particion logica con un nombre ya utilizado" << endl;
            return;
        }

        
        if(currentFit == 0){//Best Fit
            EmptEbr = GetPositionFFEbr(TempExt, newsize, parameters);
            parameters.fit = "b";

        }else if(currentFit == 1){//First Fit

            EmptEbr = GetPositionFFEbr(TempExt, newsize, parameters);
            parameters.fit = "f";

        }else if(currentFit == 2){//Worst Fit

        }

        int libre = (TempExt.size - CalcularSizeExt(TempExt.start, parameters, 0)) - newsize;
        if(libre < 0){
            cout << "El espacio restante en el disco no es suficiente para almacenar esta particion logica"<< endl;
            return;
        }

        for (int i = 0; i < 4; i++){
            char newtype = 'e';
            char typepart = mbr.particiones[i].type;
            if (newtype == typepart){
                EmptEbr = ActivarEBR(parameters, newsize, EmptEbr.start, EmptEbr.p_siguiente);
                rewind(dsk);
                fseek(dsk, startpoint, SEEK_SET);
                fwrite(&EmptEbr, sizeof(EBR), 1, dsk);
                break;
            }
        }
    }

    fclose(dsk);
}

int Fdisk::CalcularSizeExt(int start,Parametros parameters, int ocupado){

    string path = parameters.direccion;
    int Occupied = ocupado;
    FILE* dsk = fopen(path.c_str(), "rb+");
    EBR tempebr;
    int startpoint = start;
    fseek(dsk, startpoint, SEEK_SET);


    fread(&tempebr, sizeof(tempebr), 1, dsk);

    if(tempebr.status == '1'){  
        Occupied =+ tempebr.size;
        CalcularSizeExt(tempebr.p_siguiente, parameters, Occupied);
    }

    return Occupied;
}

EBR Fdisk::ActivarEBR(Parametros parameters, int size, int startpoint, int siguiente){
    EBR oldPart;

    for (int i = 0; i < sizeof(parameters.nombre); i++){
        oldPart.name[i] = parameters.nombre[i];
    }

    oldPart.fit = parameters.fit[0];
    oldPart.size = size;
    oldPart.status = '1';
    oldPart.start = startpoint;
    oldPart.p_siguiente = -1;

    return oldPart;

}


Particion Fdisk::ActivarParticion(Particion oldPart, Parametros parameters, int size, int startpoint){
   

    for (int i = 0; i < sizeof(parameters.nombre); i++){
        oldPart.name[i] = parameters.nombre[i];
    }

    oldPart.fit = parameters.fit[0];
    oldPart.size = size;
    oldPart.status = '1';
    oldPart.type = parameters.type[0];
    oldPart.start = startpoint;

    return oldPart;
}


int Fdisk::GetPositionBF(MBR mbrActual, int Tam){
    int Pos;
    int Posmbr = 0;
    MBR AllArrays;
    bool Discovacio = IstheDiskEmpty(mbrActual);
    
    if (Discovacio){
        Pos = Pos + 1;
        return Pos;
    }

    for (int i = 0; i < 4; i++){
        if(mbrActual.particiones[i].status =='1'){
            AllArrays.particiones[Posmbr] = (mbrActual.particiones[i]);
            Posmbr += 1;
        }
    }

    AllArrays = OrdenarArray(AllArrays);

    int BestPosition = 0;

    for (int i = 1; i < 4; i++){
        Particion Actual = AllArrays.particiones[i];
        Particion Anterior = AllArrays.particiones[i-1];

        if(Actual.status != '0'){
            int fin = Anterior.size + Anterior.start;
            if(fin + Tam < Actual.start){
                if (BestPosition != 0){
                    if(fin - Actual.start < BestPosition)
                        BestPosition = fin + Tam;
                }else{
                    BestPosition = fin;
                }
            }
        }else{
            return Anterior.size + Anterior.start;
        }
    }

    return BestPosition;
}

bool Fdisk::IstheDiskEmpty(MBR mbrActual){

    bool Discovacio = true;
    for (int i = 0; i < 4; i++){
        if (mbrActual.particiones[i].status =='1'){
            Discovacio = false;
            return Discovacio;
        }
    }
    return Discovacio;

}

int Fdisk::GetPositionFF(MBR mbrActual, int Tam){
    int Pos;
    int Posmbr = 0;
    MBR AllArrays;
    bool Discovacio = IstheDiskEmpty(mbrActual);
    
    if (Discovacio){
        Pos = Pos + 1;
        return Pos;
    }
    

    for (int i = 0; i < 4; i++){
        if(mbrActual.particiones[i].status =='1'){
            AllArrays.particiones[Posmbr] = (mbrActual.particiones[i]);
            Posmbr += 1;
        }
    }

    AllArrays = OrdenarArray(AllArrays);

    for (int i = 1; i < 4; i++){
        Particion Actual = AllArrays.particiones[i];
        Particion Anterior = AllArrays.particiones[i-1];

        if(Actual.status != '0'){
            int fin = Anterior.size + Anterior.start;
            if(fin + Tam < Actual.start){
                return fin;
            }
        }
    }
    

    
    return 0;
}


MBR Fdisk::OrdenarArray(MBR AllArrays){//Ordena las particiones para poder encontrar un lugar donde puda ser almacenada la particion

    int n = 4;

    int i,j;
    MBR temp;
    int postemp = 0;

    for(i=1;i<n;i++)
        for(j=n-1;j>=i;j--)
            if(AllArrays.particiones[j-1].start > AllArrays.particiones[j].start and AllArrays.particiones[j-1].start != 0 and AllArrays.particiones[j].start != 0){
                temp.particiones[postemp]=AllArrays.particiones[j-1];
                AllArrays.particiones[j-1]=AllArrays.particiones[j];
                AllArrays.particiones[j]=temp.particiones[postemp];
                postemp += 1;
            }

    return AllArrays;

}



int Fdisk::tamparticiones(MBR TempMBR){ // Se obtiene el tama;o de todas las particiones juntas

    int totalsize;

    for (const auto &i : TempMBR.particiones)
    {
        totalsize += i.size;
    }

    return totalsize;
}


EBR Fdisk::GetPositionFFEbr(Particion PartExt, int Tam,Parametros parameters){//Devuleve la primera posicion disponible en el EBR
    int Posmbr = 0;
    bool Discovacio = false;

    string path = parameters.direccion;
    int Occupied = 0;
    EBR WithPos;
    FILE* dsk = fopen(path.c_str(), "rb+");
    EBR tempebr;
    int startpoint = PartExt.start;


    if( CalcularSizeExt(startpoint, parameters, 0) == 0){
        Discovacio = true;
    }
    
    if (Discovacio){
        WithPos.start = startpoint;
        WithPos.p_siguiente = -1;
        return WithPos;
    }

    fseek(dsk, startpoint, SEEK_SET);
    fread(&tempebr, sizeof(tempebr), 1, dsk);

    while ((tempebr.status == '1')){
       Posmbr += 1;
       fseek(dsk, tempebr.p_siguiente, SEEK_SET);
       fread(&tempebr, sizeof(tempebr), 1, dsk);
    }

    if(Posmbr == 0){
        WithPos.start = startpoint;
        WithPos.p_siguiente = -1;
        return WithPos;
    }

    EBR lista[Posmbr];
    Posmbr = 0;
    fseek(dsk, startpoint, SEEK_SET);
    fread(&tempebr, sizeof(tempebr), 1, dsk);

    while (tempebr.status == '1'){
        lista[Posmbr] = tempebr;
        Posmbr =+ 1;
        fseek(dsk, tempebr.p_siguiente, SEEK_SET);
        fread(&tempebr, sizeof(tempebr), 1, dsk);
    }

    EBR AllArrays[Posmbr];
    memcpy(AllArrays, lista, sizeof(EBR));
    int n = Posmbr;
    int i,j;
    EBR temp[n];
    int postemp = 0;

    for(i=1;i<n;i++)
        for(j=n-1;j>=i;j--)
            if(AllArrays[j-1].start > AllArrays[j].start and AllArrays[j-1].start != 0 and AllArrays[j].start != 0){
            temp[postemp]=AllArrays[j-1];
            AllArrays[j-1]=AllArrays[j];
            AllArrays[j]=temp[postemp];
            postemp += 1;
        }

    for (int i = 1; i < Posmbr; i++){
        EBR Actual = AllArrays[i];
        EBR Anterior = AllArrays[i-1];

        if(Actual.status == '1'){
            int fin = Anterior.size + Anterior.start;
            if(fin + Tam < Actual.start){
                if(Anterior.p_siguiente == -1){
                    Anterior.p_siguiente = fin;
                    WithPos.p_siguiente = -1;
                    fseek(dsk, Anterior.start, SEEK_SET);
                    fwrite(&Anterior, sizeof(EBR), 1, dsk);
                }else{
                    WithPos.p_siguiente = Anterior.p_siguiente;
                    Anterior.p_siguiente = fin;
                    WithPos.start = fin;
                    fseek(dsk, Anterior.start, SEEK_SET);
                    fwrite(&Anterior, sizeof(EBR), 1, dsk);
                }
                return WithPos;
            }
        }
    }
    AllArrays[0].p_siguiente = AllArrays[0].start + AllArrays[0].size;
    fseek(dsk, AllArrays[0].start, SEEK_SET);
    fwrite(&AllArrays[0], sizeof(EBR), 1, dsk);

    WithPos.start = AllArrays[0].start + AllArrays[0].size;
    WithPos.p_siguiente = -1;
    return WithPos;
}

int Fdisk::SearchNameEBR(Particion PartExt, string Nombre, Parametros parameters){//Busca coincidencia con el nombre enviado, retorna 1 si lo encuentra
    string path = parameters.direccion;
    int Occupied = 0;
    FILE* dsk = fopen(path.c_str(), "rb+");
    EBR tempebr;
    int startpoint = PartExt.start;

    fseek(dsk, startpoint, SEEK_SET);
    fread(&tempebr, sizeof(tempebr), 1, dsk);

    while ((tempebr.status == '1')){
        if(tempebr.name == Nombre)
            return 1;
       fseek(dsk, tempebr.p_siguiente, SEEK_SET);
       fread(&tempebr, sizeof(tempebr), 1, dsk);
    }

    return 0;

}

EBR Fdisk::GetPositionBFEbr(Particion PartExt, int Tam,Parametros parameters){//Devuleve la mejor posicion disponible en el EBR
    int Posmbr = 0;
    bool Discovacio = false;
    EBR WithPos;

    string path = parameters.direccion;
    int Occupied = 0;
    FILE* dsk = fopen(path.c_str(), "rb+");
    EBR tempebr;
    int startpoint = PartExt.start;

    if( CalcularSizeExt(startpoint, parameters, 0) == 0){
        Discovacio = true;
    }
    
    if (Discovacio){
        WithPos.start = startpoint;
        WithPos.p_siguiente = -1;
        return WithPos;
    }
    fseek(dsk, startpoint, SEEK_SET);
    fread(&tempebr, sizeof(tempebr), 1, dsk);

    while ((tempebr.status == '1')){
       Posmbr += 1;
       fseek(dsk, tempebr.p_siguiente, SEEK_SET);
       fread(&tempebr, sizeof(tempebr), 1, dsk);
    }

    if(Posmbr == 0){
        WithPos.start = startpoint;
        WithPos.p_siguiente = -1;
        return WithPos;
    }

    EBR lista[Posmbr];
    Posmbr = 0;
    fseek(dsk, startpoint, SEEK_SET);
    fread(&tempebr, sizeof(tempebr), 1, dsk);

    while (tempebr.status == '1'){
        lista[Posmbr] = tempebr;
        Posmbr =+ 1;
        fseek(dsk, tempebr.p_siguiente, SEEK_SET);
        fread(&tempebr, sizeof(tempebr), 1, dsk);
    }

    EBR AllArrays[Posmbr];
    memcpy(AllArrays, lista, sizeof(EBR));
    int n = Posmbr;
    int i,j;
    EBR temp[n];
    int postemp = 0;

    for(i=1;i<n;i++)
        for(j=n-1;j>=i;j--)
            if(AllArrays[j-1].start > AllArrays[j].start and AllArrays[j-1].start != 0 and AllArrays[j].start != 0){
            temp[postemp]=AllArrays[j-1];
            AllArrays[j-1]=AllArrays[j];
            AllArrays[j]=temp[postemp];
            postemp += 1;
        }

    for (int i = 1; i < 4; i++){
        EBR Actual = AllArrays[i];
        EBR Anterior = AllArrays[i-1];

        if(Actual.status != '0'){
            int fin = Anterior.size + Anterior.start;
            if(fin + Tam < Actual.start){
                if (WithPos.start != 0){
                    WithPos.start = fin;
                    if(Anterior.p_siguiente != -1){
                        WithPos.p_siguiente = Anterior.p_siguiente;
                        Anterior.p_siguiente = fin;
                        fseek(dsk, Anterior.start, SEEK_SET);
                        fwrite(&Anterior, sizeof(EBR), 1, dsk);
                    }else{
                        Anterior.p_siguiente = fin;
                        WithPos.p_siguiente = -1;
                        fseek(dsk, Anterior.start, SEEK_SET);
                        fwrite(&Anterior, sizeof(EBR), 1, dsk);
                    }
                        
                }else{
                    WithPos.start = fin;
                    WithPos.p_siguiente = -1;
                }
            }
        }else{
            return WithPos;
        }
    }

    AllArrays[0].p_siguiente = AllArrays[0].start + AllArrays[0].size;
    fseek(dsk, AllArrays[0].start, SEEK_SET);
    fwrite(&AllArrays[0], sizeof(EBR), 1, dsk);

    WithPos.start = AllArrays[0].start + AllArrays[0].size;
    WithPos.p_siguiente = -1;

    return WithPos;
}

void Fdisk::AddSpace(Parametros parameters){
    string path = parameters.direccion;
    Particion empt;
    MBR mbrActual;
    int maxsize;
    string s_a;
    MBR AllArrays;
    FILE* dsk = fopen(path.c_str(), "rb+");
    rewind(dsk);
    fread(&mbrActual, sizeof(mbrActual), 1, dsk);
    int extstart = 0;
    bool Discovacio = IstheDiskEmpty(mbrActual);
    
    if (Discovacio){
        cout<< "No se pudo encontrar la particion deseada"<< endl;
        return;
    }

    for (int i = 0; i < 4; i++){
        if(mbrActual.particiones[i].status =='1'){
            int tam = parameters.nombre.length();
            s_a = convertToString(mbrActual.particiones[i].name, tam);
            if(mbrActual.particiones[i].type == 'e'){
                extstart = mbrActual.particiones[i].start;
                maxsize = mbrActual.particiones[i].size;
            }
            if(s_a == parameters.nombre){
                string opcion;
                cout << "La particion fue encontrada, agregar " + parameters.add+ " A la particion encontgrada? (S/N)"<<endl;
                cin >> opcion;
                if(opcion == "S" or opcion == "s"){
                    AllArrays = OrdenarArray(mbrActual);
                    for (int j = 0; j < 4; j++){
                        if(AllArrays.particiones[j].name == parameters.nombre){
                            if(j< 3){
                                if(mbrActual.particiones[i].size + stoi(parameters.add) + mbrActual.particiones[i].start < AllArrays.particiones[j+1].start){
                                    mbrActual.particiones[i].size += stoi(parameters.add);
                                    rewind(dsk);
                                    fwrite(&mbrActual, sizeof(MBR), 1, dsk);
                                    fclose(dsk);
                                    return;
                                }
                            }else{
                                if(mbrActual.particiones[i].size + stoi(parameters.add) + mbrActual.particiones[i].start < mbrActual.tamano){
                                    mbrActual.particiones[i].size += stoi(parameters.add);
                                    rewind(dsk);
                                    fwrite(&mbrActual, sizeof(MBR), 1, dsk);
                                    fclose(dsk);
                                    return;
                                }
                            }
                        }
                    }
                }
                return;
            }
        }
    }

    if(extstart != 0){
        AddLogic(extstart, maxsize, parameters.nombre, path);
        return;
    }

    cout<<"No se pudo encontrar la particion deseada"<<endl;
    return;
}

void Fdisk::AddLogic(int startpoint, int Maxsize, string name, string dir){

    string path = dir;
    EBR empt;
    FILE* dsk = fopen(path.c_str(), "rb+");
    EBR tempebr;
    fseek(dsk, startpoint, SEEK_SET);
    string s_a;
    fread(&tempebr, sizeof(tempebr), 1, dsk);
    int tam = name.length();
    s_a = convertToString(tempebr.name, tam);

    if(s_a == name){
        string opcion;
        cout << "La particion fue encontrada, agregar el espacio? (S/N)"<<endl;
        cin >> opcion;
        if(opcion == "S" or opcion == "s"){
                tempebr = empt;
                rewind(dsk);
                fwrite(&tempebr, sizeof(EBR), 1, dsk);
                fclose(dsk);
                return;
        }
    }else if(tempebr.p_siguiente != -1){
        AddLogic(tempebr.p_siguiente, Maxsize, name, dir);
        return;
    }else{
        cout<< "No se pudo encontrar la particion deseada"<<endl;
    }
    return;
}