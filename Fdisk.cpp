#include <iostream>
#include "Estructuras.cpp"
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
    int GetPositionFF(MBR mbrActual, int Tam);
    MBR OrdenarArray(MBR AllArrays);

    Comando cmd;
    Compartido compart;
};

bool Fdisk::existeDisco(string path)
{

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

void Fdisk::CrearParticion(Parametros parameters)
{ // Crea las particiones para los discos

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

    if(currentFit = 0){//Best Fit


    }else if(currentFit = 1){//First Fit
        startpoint = GetPositionFF(mbr, newsize);
        parameters.fit = "f";
            


    }else if(currentFit = 2){//Worst Fit


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
        }
    }

    fclose(dsk);
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

int Fdisk::GetPositionFF(MBR mbrActual, int Tam){
    int Pos;
    MBR AllArrays;
    bool Discovacio = true;

    Pos = sizeof(mbrActual);
    int Posmbr = 0;

    for (int i = 0; i < 4; i++){
        if (mbrActual.particiones[i].status =='1'){
            Discovacio = false;
        }
    }
    
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
        }else{
            return Anterior.size + Anterior.start;
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