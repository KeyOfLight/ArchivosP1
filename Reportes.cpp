#include <iostream>
#include <fstream>
#include "Estructuras.cpp"

using namespace std;


class Reportes{
    public:
    void GenerarReporte(string path);

    Comando cmd;

};

void Reportes::GenerarReporte(string path){

    ifstream dsk(path.c_str(), ios::out | ios::binary);

    if(!dsk){
        cout<<"Error, el archivo no pudo ser abierto."<<endl;
        return;
    }

    MBR mbr;

    dsk.read((char *) &mbr, sizeof(MBR));
    dsk.close();

    cout << "Size: " << mbr.tamano << endl;
    cout << "Fecha: " << mbr.fecha_creacion << endl;
    cout << "Signature: " << mbr.disk_signature  << endl;
    cout << "Particion 1: " << mbr.particiones[0].name  << endl;
    cout << "Particion 2: " << mbr.particiones[1].name << endl;
    cout << "Particion 3: " << mbr.particiones[2].name  << endl;


}