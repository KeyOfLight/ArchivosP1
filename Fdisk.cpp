#include <iostream>
#include "Estructuras.cpp"
#include <fstream>
using namespace std;

class Fdisk
{
public:
    void CrearParticion(Parametros parameters);
    bool existeDisco(string path);
    int tamparticiones(MBR TempMbr);
    Particion ActivarParticion(Particion oldPart, Parametros parameters, int size);

    Comando cmd;
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
    //ifstream dsk(path.c_str(), ios::out | ios::binary);

    /*
    if (!dsk)
    {
        cout << "Error, el archivo no pudo ser abierto." << endl;
        return;
    }
    */

    MBR mbr;

    FILE* dsk = fopen(path.c_str(), "rb+");

    if (dsk != NULL) {
        rewind(dsk);
        fread(&mbr, sizeof(mbr), 1, dsk);
    } else {
        cout << endl << " *** Error el path del disco no existe *** " << endl << endl ;
    }

    //dsk.read((char *)&mbr, sizeof(MBR)); // Se obtienen los datos del mbr deseado

    
    int pos = fseek(dsk,0,mbr);

    if (newsize < 0 and newsize > mbr.tamano)
    {
        cout << "Se debe de crear una particion que ocupe mas de 0 bytes o que se menor que el disco" << endl;
        fclose(dsk);
        return;
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
                    
                    mbr.particiones[i] = ActivarParticion(mbr.particiones[i], parameters, newsize);

                    break;
                }
            }
        }
    }

    fclose(dsk);
}

Particion Fdisk::ActivarParticion(Particion oldPart, Parametros parameters, int size){
   

    for (int i = 0; i < sizeof(parameters.nombre); i++){
        oldPart.name[i] = parameters.nombre[i];
    }
    
    oldPart.fit = parameters.fit[0];
    oldPart.size = size;
    oldPart.status = '1';
    oldPart.type = parameters.type[0];

    return oldPart;
    //Agregar start//////////////////////////////////////////////////////////////////////////////////////////////////
}

int Fdisk::tamparticiones(MBR TempMBR)
{ // Se obtiene el tama;o de todas las particiones juntas

    int totalsize;

    for (const auto &i : TempMBR.particiones)
    {
        totalsize += i.size;
    }

    return totalsize;
}