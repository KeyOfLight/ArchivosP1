#include <iostream>
#include <fstream>
#include "Estructuras.cpp"

using namespace std;


class Reportes{
    public:
    void GenerarReporte(Parametros parameters);
    string RecorrerExt(int start,Parametros parameters);
    void ReportMBR(Parametros parameters);
    void ReportDisk(Parametros parameters);

    int ContarEBRs(int start,Parametros parameters, int ocupado);
    string DiskEBR(int start,Parametros parameters, MBR mbr, int written, int total);

    Comando cmd;

};

void Reportes::GenerarReporte(Parametros parameters){

    if(parameters.nombre == "mbr"){
        ReportMBR(parameters);
        return;
    }else if(parameters.nombre == "disk"){
        ReportDisk(parameters);
    }



};

void Reportes::ReportDisk(Parametros parameters){
    string path = parameters.direccion;
    string Diagrama = "digraph DSK{\n node [shape=plaintext];\n ";
    Particion TempExt;

    Diagrama += "struct3 [label=<\n<TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"1\" CELLPADDING=\"4\">\n";

    FILE* dsk = fopen(path.c_str(), "rb+");
    MBR mbr;
    fseek(dsk, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, dsk);
    int porcentaje;
    int ocupado = 0;

    Diagrama += "<TR>\n<TD BGCOLOR=\"purple\" ROWSPAN=\"2\">MBR</TD>\n";

    for (int i = 0; i < 4; i++){
        Particion act = mbr.particiones[i];
        if(act.status == '1'){
            char newtype = 'e';
            char typepart = mbr.particiones[i].type;
            if (newtype == typepart){
                TempExt = mbr.particiones[i];
                int Externos;
                Externos = ContarEBRs(act.start, parameters, 1);
                ocupado += act.size;
                Diagrama += "<TD COLSPAN=\""+to_string(Externos*2)+"\">Extendida</TD>\n";
            }else{
                porcentaje = (act.size * 100) / mbr.tamano;
                ocupado += act.size;
                Diagrama += "<TD ROWSPAN=\"2\">Primaria <BR/>"+ to_string(porcentaje)+"%</TD>\n";
            }
        }else{
            porcentaje = (mbr.tamano - ocupado) * 100 / mbr.tamano;
            Diagrama += "<TD ROWSPAN=\"2\">Libre <BR/>"+ to_string(porcentaje)+"%</TD>\n";
        }
    }
    Diagrama += "</TR>\n";
    Diagrama += "<TR>\n";
    Diagrama += DiskEBR(TempExt.start, parameters, mbr, 0, TempExt.size);
    Diagrama += "</TR>\n</TABLE>>];\n}";

    ofstream MyFile("DSk.dot");
    MyFile << Diagrama;
    MyFile.close();

    string com = "dot -Tpdf DSk.dot -o DSk.pdf";
    const char *cmd = com.c_str();
    system(cmd);
    
};

string Reportes::DiskEBR(int start,Parametros parameters, MBR mbr, int written, int total){

    string path = parameters.direccion;
    int escrito = written;
    FILE* dsk = fopen(path.c_str(), "rb+");
    EBR tempebr;
    string Diagrama = "";
    int startpoint = start;
    rewind(dsk);
    fseek(dsk, startpoint, SEEK_SET);
    fread(&tempebr, sizeof(EBR), 1, dsk);


    if(tempebr.status == '1'){ 

        string stado(1, tempebr.status);
        int porcentaje = (tempebr.size * 100) / mbr.tamano;
        escrito += tempebr.size;
        Diagrama += "<TD BGCOLOR=\"lightblue\" >EBR</TD><TD>Logica <BR/> " + to_string(porcentaje) + " % </TD>";
        Diagrama += DiskEBR(tempebr.p_siguiente, parameters, mbr, escrito, total);
    }else{

        int porcentaje = (total - escrito) * 100 / mbr.tamano;
        Diagrama += "<TD BGCOLOR=\"lightblue\" >EBR</TD><TD>Libre  <BR/> " + to_string(porcentaje) + " % </TD>";
    }

    return Diagrama;

};

void Reportes::ReportMBR(Parametros parameters){
    string path = parameters.direccion;
    string Diagrama = "digraph MBR{\n rankdir=LR\n node [shape=Mrecord];\n ";
    Particion TempExt;

    ifstream dsk(path.c_str(), ios::out | ios::binary);
    MBR mbr;

    dsk.read((char *) &mbr, sizeof(MBR));
    dsk.close();

    cout << "============================"<<endl;
    cout << "Size MBR: " << mbr.tamano << endl;
    cout << "Fecha de creacion: " << mbr.fecha_creacion << endl;
    cout << "Signature dsk: " << mbr.disk_signature  << endl;

    Diagrama += "struct1 [label=\" {Reporte MBR | {Mbr Tam| Firma | Fecha de creacion}| {" + to_string(mbr.tamano) + " | " + to_string(mbr.disk_signature) + " | " + mbr.fecha_creacion + " }}";

    for (int i = 0; i < 4; i++){
        cout << "============================"<<endl;

        cout << "Particion"+ to_string(i+1)+" nombre: " << mbr.particiones[i].name  << endl;
        cout << "Particion"+ to_string(i+1)+" fit: " << mbr.particiones[i].fit  << endl;
        cout << "Particion"+ to_string(i+1)+" Size: " << mbr.particiones[i].size  << endl;
        cout << "Particion"+ to_string(i+1)+" Punto de incio: " << mbr.particiones[i].start  << endl;
        cout << "Particion"+ to_string(i+1)+" status: " << mbr.particiones[i].status  << endl;
        cout << "Particion"+ to_string(i+1)+" Tipo: " << mbr.particiones[i].type  << endl;

        string stado(1, mbr.particiones[i].status);

        Diagrama += "| {Particion | {Status | Tipo | Fit | StatPoint | Size | Nombre}| {" + stado + " | " + mbr.particiones[i].type + " | " + mbr.particiones[i].fit + " | " + to_string(mbr.particiones[i].start) + " | " + to_string(mbr.particiones[i].size) + " | " + mbr.particiones[i].name +" }}";


        char newtype = 'e';
        char typepart = mbr.particiones[i].type;
        if (newtype == typepart){
            TempExt = mbr.particiones[i];
        }
        
    }
    char newtype = 'e';
    if (TempExt.type == newtype){
        cout << "============================"<<endl;
        cout << "EBR"<<endl;
        Diagrama += RecorrerExt(TempExt.start,parameters);
    }

    Diagrama += "\"]}";

    ofstream MyFile("MBR.dot");
    MyFile << Diagrama;
    MyFile.close();

    string com = "dot -Tpdf MBR.dot -o MBR.pdf";
    const char *cmd = com.c_str();
    system(cmd);

}



string Reportes::RecorrerExt(int start,Parametros parameters){

    string path = parameters.direccion;
    FILE* dsk = fopen(path.c_str(), "rb+");
    EBR tempebr;
    string Diagrama = "";
    int startpoint = start;
    rewind(dsk);
    fseek(dsk, startpoint, SEEK_SET);
    fread(&tempebr, sizeof(EBR), 1, dsk);

    if(tempebr.status == '1'){ 
        cout << "Particion nombre: " << tempebr.name  << endl;
        cout << "Particion fit: " << tempebr.fit  << endl;
        cout << "Particion Size: " << tempebr.size  << endl;
        cout << "Particion Punto de incio: " << tempebr.start  << endl;
        cout << "Particion status: " << tempebr.status  << endl;

        string stado(1, tempebr.status);
        Diagrama += "| {Particion | {Status | Fit | StatPoint | Size | Nombre}| {" + stado + " | " + tempebr.fit + " | " + to_string(tempebr.start) + " | " + to_string(tempebr.size) + " | " + tempebr.name+" }}";
        Diagrama += RecorrerExt(tempebr.p_siguiente, parameters);
    }

    return Diagrama;

};


int Reportes::ContarEBRs(int start,Parametros parameters, int ocupado){

    string path = parameters.direccion;
    int Occupied = ocupado;
    FILE* dsk = fopen(path.c_str(), "rb+");
    EBR tempebr;
    int startpoint = start;
    fseek(dsk, startpoint, SEEK_SET);


    fread(&tempebr, sizeof(EBR), 1, dsk);

    if(tempebr.status == '1'){  
        Occupied += 1;
        return ContarEBRs(tempebr.p_siguiente, parameters, Occupied);
    }

    return Occupied;
}


