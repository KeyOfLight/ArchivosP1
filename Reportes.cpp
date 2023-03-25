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
    void BlockReportes(Parametros parameters, User Uss);
    string GraphBlocks(int pos, FILE* dsk, Sblock SupBlock, bool Inodo);
    string GraphInodo(int pos, FILE* dsk, Sblock SupBlock, bool Inodo);

    string GraphFileBlocks(I_node InodoArchivo, int BlockStartPoint, FILE* dsk, int InodoStart);
    string ReadPointer(BloqueApuntadores Apuntador, FILE* dsk, int BlockStartPoint);

    void RerportBMInodos(Parametros parameters, User Uss);
    void RerportBMBloques(Parametros parameters, User Uss);
    void ReportSupBlock(Parametros parameters, User Uss);


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
    }else if(parameters.nombre == "block"){
        BlockReportes(parameters, Uss);
    }else if(parameters.nombre == "bm_inode"){
        RerportBMInodos(parameters, Uss);
    }else if(parameters.nombre == "bm_block"){
        RerportBMBloques(parameters, Uss);
    }else if(parameters.nombre == "sb"){
        ReportSupBlock(parameters, Uss);
    }


};

void Reportes::ReportSupBlock(Parametros parameters, User Uss){
    int pos;
    string path = parameters.direccion;
    int startpoint = Uss.startpoint;
    string Diagrama = "digraph SBloques{\n node [shape=plaintext];\nrankdir=LR;\n ";
    FILE* dsk = fopen(path.c_str(), "rb+");
    fseek(dsk, startpoint, SEEK_SET);
    //Leer Superblock
    Sblock super_block;
    fread(&super_block, sizeof(Sblock), 1, dsk);
    
    Diagrama += "SBlock[label=<\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"1\" CELLPADDING=\"4\">\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"purple\" COLSPAN=\"2\">Super Bloque</TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_filesystem_type</TD><TD>" + to_string(super_block.s_filesystem_type) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_inodes_count</TD><TD>" + to_string(super_block.s_inodes_count) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_blocks_count</TD><TD>" + to_string(super_block.s_blocks_count) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_free_blocks_count</TD><TD>" + to_string(super_block.s_free_blocks_count) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_free_inodes_count</TD><TD>" + to_string(super_block.s_free_inodes_count) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_mtime</TD><TD>" + string(super_block.s_mtime) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_umtime</TD><TD>" + string(super_block.s_umtime) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_mnt_count</TD><TD>" + to_string(super_block.s_mnt_count) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_magic</TD><TD>" + to_string(super_block.s_magic) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_inode_size</TD><TD>" + to_string(super_block.s_inode_size) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_block_size</TD><TD>" + to_string(super_block.s_block_size) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_firts_ino</TD><TD>" + to_string(super_block.s_firts_ino) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_first_blo</TD><TD>" + to_string(super_block.s_first_blo) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_bm_inode_start</TD><TD>" + to_string(super_block.s_bm_inode_start) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_bm_block_start</TD><TD>" + to_string(super_block.s_bm_block_start) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_inode_start</TD><TD>" + to_string(super_block.s_inode_start) + " </TD>\n</TR>\n";
    Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">s_block_start</TD><TD>" + to_string(super_block.s_block_start) + " </TD>\n</TR>\n";
    Diagrama += "</TABLE>>];\n";

    Diagrama += "}";

    ofstream MyFile("SBlock.dot");
    MyFile << Diagrama;
    MyFile.close();

    string com = "dot -Tpdf SBlock.dot -o SBlock.pdf";
    const char *cmd = com.c_str();
    system(cmd);

};

void Reportes::RerportBMBloques(Parametros parameters, User Uss){
    int pos;
    string path = parameters.direccion;
    int startpoint = Uss.startpoint;
    string Diagrama = "";
    FILE* dsk = fopen(path.c_str(), "rb+");
    fseek(dsk, startpoint, SEEK_SET);
    //Leer Superblock
    Sblock super_block;
    fread(&super_block, sizeof(Sblock), 1, dsk);
    char actual;

    for (int i = super_block.s_bm_block_start; i < super_block.s_inode_start; i++){
        fseek(dsk, i, SEEK_SET);
        fread(&actual, sizeof(char), 1, dsk);
        Diagrama += actual;
        if(((i-super_block.s_bm_block_start)%20) == 0 && (i-super_block.s_bm_block_start) != 0){
            Diagrama += "\n";
        }
    }

    ofstream MyFile("BMBloques.txt");
    MyFile << Diagrama;
    MyFile.close();

};

void Reportes::RerportBMInodos(Parametros parameters, User Uss){
    int pos;
    string path = parameters.direccion;
    int startpoint = Uss.startpoint;
    string Diagrama = "";
    FILE* dsk = fopen(path.c_str(), "rb+");
    fseek(dsk, startpoint, SEEK_SET);
    //Leer Superblock
    Sblock super_block;
    fread(&super_block, sizeof(Sblock), 1, dsk);
    char actual;

    for (int i = super_block.s_bm_inode_start; i < super_block.s_bm_block_start; i++){
        fseek(dsk, i, SEEK_SET);
        fread(&actual, sizeof(char), 1, dsk);
        Diagrama += actual;
        if(((i-super_block.s_bm_inode_start)%20) == 0 && (i-super_block.s_bm_inode_start) != 0){
            Diagrama += "\n";
        }
    }

    ofstream MyFile("BMInodos.txt");
    MyFile << Diagrama;
    MyFile.close();

};

void Reportes::BlockReportes(Parametros parameters, User Uss){
    int pos;
    string path = parameters.direccion;
    int startpoint = Uss.startpoint;
    string Diagrama = "digraph Bloques{\n node [shape=plaintext];\nrankdir=LR;\n ";


    FILE* dsk = fopen(path.c_str(), "rb+");
    fseek(dsk, startpoint, SEEK_SET);
    //Leer Superblock
    Sblock super_block;
    fread(&super_block, sizeof(Sblock), 1, dsk);

    I_node InodoRoot;
    fseek(dsk, super_block.s_inode_start, SEEK_SET);
    fread(&InodoRoot, sizeof(I_node), 1, dsk);

    string stado(1, InodoRoot.i_type);

    for (int i = 0; i < 15; i++){//WIP Apuntadores indirectos
        if(InodoRoot.i_block[i] != -1){
            pos = InodoRoot.i_block[i];
            Diagrama +=GraphBlocks(pos,dsk, super_block, false);
        }
    }

    Diagrama += "}";

    ofstream MyFile("BLock.dot");
    MyFile << Diagrama;
    MyFile.close();

    string com = "dot -Tpdf BLock.dot -o BLock.pdf";
    const char *cmd = com.c_str();
    system(cmd);

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



string Reportes::GraphFileBlocks(I_node InodoArchivo, int BlockStartPoint, FILE* dsk, int InodoStart){

    int contador;
    string ContenidoStr = "";

    //Leer el contenido de un bloque de archivos
    for (int i = 0; i < 12; i++){
        if(InodoArchivo.i_block[i] != -1){
            contador = InodoArchivo.i_block[i];
            ContenidoStr += "Block"+to_string(contador)+"[label=<\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"1\" CELLPADDING=\"4\">\n";
            ContenidoStr += "<TR>\n<TD BGCOLOR=\"purple\">Bloque Carpetas "+to_string(contador)+"</TD>\n</TR>\n";
            BloqueArchivos bl_archivo;
            fseek(dsk, BlockStartPoint + sizeof(BloqueCarpetas)*contador, SEEK_SET);
            fread(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);
            ContenidoStr += "<TR>\n<TD>" + string(bl_archivo.b_content) + " </TD>\n</TR>\n";
            ContenidoStr += "</TABLE>>];\n";
        }
    }

    //Si el apuntador indirecto ya tiene informacion la lee
    if(InodoArchivo.i_block[12] != -1){
        BloqueApuntadores BPointer;
        contador = InodoArchivo.i_block[12];
        ContenidoStr += "Block"+to_string(InodoArchivo.i_block[11])+" -> Block"+to_string(contador)+"\n";
        ContenidoStr += "Block"+to_string(contador)+"[label=<\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"1\" CELLPADDING=\"4\">\n";
        ContenidoStr += "<TR>\n<TD BGCOLOR=\"purple\">Bloque Carpetas "+to_string(contador)+"</TD>\n</TR>\n";

        //Se busca el bloque de apuntador
        fseek(dsk, BlockStartPoint + sizeof(BloqueApuntadores)*contador, SEEK_SET);
        fread(&BPointer, sizeof(BloqueApuntadores), 1, dsk);
        ContenidoStr += "<TR>\n<TD>" + ReadPointer(BPointer, dsk, BlockStartPoint) + " </TD>\n</TR>\n";
        ContenidoStr += "</TABLE>>];\n";
    }

    //Apuntador indirecto doble con informacion
    if(InodoArchivo.i_block[13] != -1){
        BloqueApuntadores BPointer;
        BloqueApuntadores BPointer2;
        contador = InodoArchivo.i_block[12];
        int contador2;
        bool Escrito = false;

        ContenidoStr += "Block"+to_string(InodoArchivo.i_block[12])+" -> Block"+to_string(contador)+"\n";
        ContenidoStr += "Block"+to_string(contador)+"[label=<\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"1\" CELLPADDING=\"4\">\n";
        ContenidoStr += "<TR>\n<TD BGCOLOR=\"purple\">Bloque Carpetas "+to_string(contador)+"</TD>\n</TR>\n";

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
                ContenidoStr += "<TR>\n<TD>" + ReadPointer(BPointer, dsk, BlockStartPoint) + " </TD>\n</TR>\n";
            }
        }
        ContenidoStr += "</TABLE>>];\n";
    }

    return ContenidoStr;
};


string Reportes::ReadPointer(BloqueApuntadores Apuntador, FILE* dsk, int BlockStartPoint){
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




string Reportes::GraphBlocks(int pos, FILE* dsk, Sblock SupBlock, bool Inodo){

    BloqueCarpetas bloqueCarpeta;
    int value;
    int value2;
    string Diagrama = "";
    Content Actual;
    int SearchPoint = SupBlock.s_block_start + sizeof(BloqueCarpetas) * pos;
    int posActualBloque;
    fseek(dsk, SearchPoint, SEEK_SET);
    fread(&bloqueCarpeta, sizeof(BloqueCarpetas), 1, dsk);
    int largo = sizeof(bloqueCarpeta.b_content) / sizeof(bloqueCarpeta.b_content[0]);

    if(!Inodo){
        posActualBloque = pos;
        Diagrama += "Block"+to_string(pos)+"[label=<\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"1\" CELLPADDING=\"4\">\n";
        Diagrama += "<TR>\n<TD BGCOLOR=\"purple\" COLSPAN=\"2\">Bloque Carpetas "+to_string(pos)+"</TD>\n</TR>\n";
        for (int i = 0; i < largo; i++){
            Actual = bloqueCarpeta.b_content[i];
            Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">b_name</TD><TD>" + string(Actual.b_name) + " </TD>\n</TR>\n";
            Diagrama += "<TR>\n<TD BGCOLOR=\"lightblue\">Actual.b_inodo</TD><TD>" + to_string(Actual.b_inodo) + " </TD>\n</TR>\n";
        }
        Diagrama += "</TABLE>>];\n";
    }
    for (int i = 0; i < largo; i++){
        Actual = bloqueCarpeta.b_content[i];
        char PathAct[12];
        if(Actual.b_inodo != 0 and Actual.b_inodo  != -1){
            pos = Actual.b_inodo;
            Diagrama += GraphInodo(pos, dsk, SupBlock, Inodo);
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
            if(value == 0){
                Diagrama = GraphFileBlocks(inodoArchivo, SupBlock.s_block_start, dsk, SupBlock.s_inode_start);
                return Diagrama;
            }else{
                for (int i = 0; i < 12; i++){//WIP Apuntadores Indirectos
                    if(inodoArchivo.i_block[i] != -1){
                        pos = inodoArchivo.i_block[i];
                        Diagrama += GraphBlocks(pos,dsk, SupBlock, Inodo);
                    }
                }
                return Diagrama;
            }
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
