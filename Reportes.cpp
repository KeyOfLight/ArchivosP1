#include <iostream>
#include <fstream>
#include "Estructuras.cpp"

using namespace std;


class Reportes{
    public:
    void GenerarReporte(Parametros parameters, User Uss);
    string RecorrerExt(int start,Parametros parameters);
    void ReportMBR(Parametros parameters);
    void ReportDisk(Parametros parameters);

    int ContarEBRs(int start,Parametros parameters, int ocupado);
    string DiskEBR(int start,Parametros parameters, MBR mbr, int written, int total);
    void ReportInodeRoot(Parametros parameters, User Uss);
    string GraphBlocks(int pos, FILE* dsk, Sblock SupBlock, bool Inodo);
    string GraphInodo(int pos, FILE* dsk, Sblock SupBlock, bool Inodo);


    Comando cmd;

};

void Reportes::GenerarReporte(Parametros parameters, User Uss){

    if(parameters.nombre == "mbr"){
        ReportMBR(parameters);
        return;
    }else if(parameters.nombre == "disk"){
        ReportDisk(parameters);
    }else if(parameters.nombre == "inode"){
        ReportInodeRoot(parameters, Uss);
    }

};

void Reportes::ReportInodeRoot(Parametros parameters, User Uss){
    int pos;
    string path = parameters.direccion;
    int startpoint = Uss.startpoint;
    string Diagrama = "digraph DSK{\n node [shape=plaintext];\nrankdir=LR;\n ";
    Diagrama += "InodoRoot [label=<\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"1\" CELLPADDING=\"4\">\n";


    FILE* dsk = fopen(path.c_str(), "rb+");
    fseek(dsk, startpoint, SEEK_SET);
    //Leer Superblock
    Sblock super_block;
    fread(&super_block, sizeof(Sblock), 1, dsk);

    Diagrama += "<TR>\n<TD BGCOLOR=\"purple\" COLSPAN=\"2\">Inodo Root</TD></TR>\n";
    I_node InodoRoot;
    fseek(dsk, super_block.s_inode_start, SEEK_SET);
    fread(&InodoRoot, sizeof(I_node), 1, dsk);

    string stado(1, InodoRoot.i_type);

    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_uid</TD><TD> " + to_string(InodoRoot.i_uid) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_gid</TD><TD> " + to_string(InodoRoot.i_gid) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_size</TD><TD> " + to_string(InodoRoot.i_size) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_atime</TD><TD> " + string(InodoRoot.i_atime) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_ctime</TD><TD> " + string(InodoRoot.i_ctime) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_mtime</TD><TD> " + string(InodoRoot.i_mtime) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_type</TD><TD> " + stado + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_perm</TD><TD> " + to_string(InodoRoot.i_perm) + " </TD>\n</TR>\n";

    Diagrama += "</TABLE>>];\n";



    for (int i = 0; i < 15; i++){//WIP Apuntadores indirectos
        if(InodoRoot.i_block[i] != -1){
            pos = InodoRoot.i_block[i];
            Diagrama += "InodoRoot -> Inodo";
            Diagrama +=GraphBlocks(pos,dsk, super_block, true);//Wip
        }
    }

    Diagrama += "}";

    ofstream MyFile("Inodo.dot");
    MyFile << Diagrama;
    MyFile.close();

    string com = "dot -Tpdf Inodo.dot -o Inodo.pdf";
    const char *cmd = com.c_str();
    system(cmd);
};


string Reportes::GraphBlocks(int pos, FILE* dsk, Sblock SupBlock, bool Inodo){

    BloqueCarpetas bloqueCarpeta;
    int value;
    int value2;
    string Diagrama = "";
    int SearchPoint = SupBlock.s_block_start + sizeof(BloqueCarpetas) * pos;

    fseek(dsk, SearchPoint, SEEK_SET);
    fread(&bloqueCarpeta, sizeof(BloqueCarpetas), 1, dsk);

    int largo = sizeof(bloqueCarpeta.b_content) / sizeof(bloqueCarpeta.b_content[0]);
    for (int i = 0; i < largo; i++){
        Content Actual;
        Actual = bloqueCarpeta.b_content[i];

        if(!Inodo){
            //Graficar Bloque WIP
        }else{
            char PathAct[12];
            if(Actual.b_inodo != 0 and Actual.b_inodo  != -1){
                pos = Actual.b_inodo;
                Diagrama += GraphInodo(pos, dsk, SupBlock, Inodo);
            }
        }
    }
    return Diagrama;
};

string Reportes::GraphInodo(int pos, FILE* dsk, Sblock SupBlock, bool Inodo){
    I_node inodoArchivo;
    string Diagrama = "";
    char Archivo;
    int value;
    fseek(dsk, SupBlock.s_inode_start + sizeof(I_node) * pos, SEEK_SET);
    fread(&inodoArchivo, sizeof(I_node), 1, dsk);
    strcpy(&Archivo, "1");
    value = strcmp(&inodoArchivo.i_type, &Archivo);

    if(!Inodo){  
        if(value == 0){
            //Diagrama += ReadFileBlocks(inodoArchivo, SupBlock.s_block_start, dsk, SupBlock.s_inode_start);//WIP
        }
    }else{
        string stado(1, inodoArchivo.i_type);
        Diagrama += to_string(pos)+"\n";
        Diagrama += "Inodo"+to_string(pos)+"[label=<\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"1\" CELLPADDING=\"4\">\n";
        Diagrama += "<TR>\n<TD BGCOLOR=\"purple\" COLSPAN=\"2\">Inodo"+to_string(pos)+"</TD>\n</TR>\n";
        Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_uid</TD><TD>" + to_string(inodoArchivo.i_uid) + " </TD>\n</TR>\n";
        Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_gid</TD><TD>" + to_string(inodoArchivo.i_gid) + " </TD>\n</TR>\n";
        Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_size</TD><TD>" + to_string(inodoArchivo.i_size) + " </TD>\n</TR>\n";
        Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_atime</TD><TD>" + string(inodoArchivo.i_atime) + " </TD>\n</TR>\n";
        Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_ctime</TD><TD>" + string(inodoArchivo.i_ctime) + " </TD>\n</TR>\n";
        Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_mtime</TD><TD>" + string(inodoArchivo.i_mtime) + " </TD>\n</TR>\n";
        Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_type</TD><TD>" + stado + " </TD>\n</TR>\n";
        Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">i_perm</TD><TD>" + to_string(inodoArchivo.i_perm) + " </TD>\n</TR>\n";
        Diagrama += "</TABLE>>];\n";
        if(value == 0){
            return Diagrama;
        }else{

            for (int i = 0; i < 12; i++){//WIP Apuntadores Indirectos
                if(inodoArchivo.i_block[i] != -1){
                    Diagrama += "Inodo"+to_string(pos) +" -> Inodo";
                    pos = inodoArchivo.i_block[i];
                    Diagrama += GraphBlocks(pos,dsk, SupBlock, Inodo);
                }
            }
            return Diagrama;
        }
    }

    return Diagrama;
};

void Reportes::ReportDisk(Parametros parameters){
    string path = parameters.direccion;
    string Diagrama = "digraph DSK{\n node [shape=plaintext];\n ";
    Particion TempExt;

    Diagrama += "struct3 [label=<\n<TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"1\" CELLPADDING=\"4\">\n";
    

    FILE* dsk = fopen(path.c_str(), "rb+");
    MBR mbr;
    rewind(dsk);
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


