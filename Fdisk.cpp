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

    ifstream dsk(path.c_str(), ios::out | ios::binary);

    if (!dsk)
    {
        cout << "Error, el archivo no pudo ser abierto." << endl;
        return;
    }

    MBR mbr;

    dsk.read((char *)&mbr, sizeof(MBR)); // Se obtienen los datos del mbr deseado

    if (newsize < 0 and newsize > mbr.tamano)
    {
        cout << "Se debe de crear una particion que ocupe mas de 0 bytes o que se menor que el disco" << endl;
        dsk.close();
        return;
    }

    if (parameters.type == "p" or parameters.type == ""){
        int filledsize = tamparticiones(mbr); /// Obtiene todo el tama;o ocupado por las particiones ya existentes
        if (newsize < (mbr.tamano - filledsize))
        {
            Particion EmptPart;
            for (int i = 0; i < 4; i++){
                int result1 = strcmp(mbr.particiones[i].name, parameters.nombre);
                if(result == 0){
                    cout << "No se puede crear una particion con un nombre ya utilizado" << endl;
                    return;
                }

            }

            for (int i = 0; i < 4; i++)
            {
                int result = strcmp(mbr.particiones[i].name, EmptPart.name);
                if (result == 0)
                {
                    mbr.particiones[i].name = parameters.nombre; 
                    break;
                }
            }
        }
    }

    dsk.close();
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