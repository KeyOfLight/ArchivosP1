#include <iostream>
#include "Estructuras.cpp"
#include <bits/stdc++.h>
#include "Compartido.cpp"
#include <fstream>
#include <vector>
using namespace std;

class Mkfs{
    public:

    void StartMkfs(vector<Mounter> Montados, Parametros parameters);
    void CreateExt2(Mounter Particion);
    void CreateExt3(Mounter Particion);

};

void Mkfs::StartMkfs(vector<Mounter> Montados, Parametros parameters){

    for (int i = 0; i < Montados.size(); i++){
        if(parameters.nombre == Montados[i].id){
            if(parameters.fs == "ext3"){
                CreateExt3(Montados[i]);
                return;
            }
            CreateExt2(Montados[i]);
        }
    }
};

void Mkfs::CreateExt2(Mounter Particion){
    double n = 0;
    string path;
    path = Particion.path;
    int Estructs_Num;
    time_t t;
    struct tm *tm;
    char fechayhora[20];
    char nulo = '0';

    t = time(NULL);
    tm = localtime(&t);
    strftime(fechayhora, 20, "%d/%m/%Y %H:%M:%S", tm);

    int startpoint = Particion.startpoint;
    n =(Particion.size - sizeof(Sblock)) / (4 + sizeof(I_node) + 3*sizeof(BloqueArchivos));

    if (n < 1)
    {
        cout << "El tamaño de la partición es muy pequeño para contener el sistema de archivos" << endl;
        return;
    }

    Estructs_Num = int(floor(n));
    int num_block = 3*Estructs_Num;

    FILE* dsk = fopen(path.c_str(), "rb+");

    Sblock SupBlock;

    SupBlock.s_filesystem_type = 2;
    SupBlock.s_inodes_count = Estructs_Num;
    SupBlock.s_blocks_count = num_block;
    SupBlock.s_free_inodes_count = Estructs_Num-2;
    SupBlock.s_free_blocks_count = num_block-2;
    strcpy(SupBlock.s_mtime, fechayhora);
    strcpy(SupBlock.s_umtime, fechayhora);
    SupBlock.s_mnt_count = 0;
    SupBlock.s_magic = 0xEF53;
    SupBlock.s_inode_size = sizeof(I_node);
    SupBlock.s_block_size = 64;
    SupBlock.s_firts_ino = 2;
    SupBlock.s_first_blo = 2;

     int PosSupB = startpoint;
    if(Particion.type == 'e'){
        PosSupB = startpoint + sizeof(EBR);
        SupBlock.s_block_start = startpoint + sizeof(EBR) + int(sizeof(Sblock)) + Estructs_Num + num_block + (int(sizeof(I_node))*Estructs_Num);
        SupBlock.s_bm_inode_start = startpoint + int(sizeof(Sblock) + sizeof(EBR));
        SupBlock.s_bm_block_start = startpoint + int(sizeof(Sblock)) + Estructs_Num + sizeof(EBR);
        SupBlock.s_inode_start = startpoint + int(sizeof(Sblock)) + Estructs_Num + num_block + sizeof(EBR);

        for (int i = 0; i < Particion.size - sizeof(EBR); i++){
            fwrite(&nulo, sizeof(nulo), 1, dsk);
            fseek(dsk, PosSupB + i, SEEK_SET);
        }

        fseek(dsk, PosSupB, SEEK_SET);
    }else{
        SupBlock.s_block_start = startpoint + int(sizeof(Sblock)) + Estructs_Num + num_block + (int(sizeof(I_node))*Estructs_Num);
        SupBlock.s_bm_inode_start = startpoint + int(sizeof(Sblock));
        SupBlock.s_bm_block_start = startpoint + int(sizeof(Sblock)) + Estructs_Num;
        SupBlock.s_inode_start = startpoint + int(sizeof(Sblock)) + Estructs_Num + num_block;

        for (int i = 0; i < Particion.size; i++){
            fwrite(&nulo, sizeof(nulo), 1, dsk);
            fseek(dsk, PosSupB + i, SEEK_SET);
        }
        fseek(dsk, startpoint, SEEK_SET);
    }

    fwrite(&SupBlock, sizeof(Sblock), 1, dsk);

    //Seek inodos

    char inodos[int(n)];
    char bloques[3 * int(n)];

    for (int i = 0; i < n; i++)
    {
        inodos[i] = '0';
    }

    inodos[0] = '1';
    inodos[1] = '1';
    fseek(dsk, PosSupB + sizeof(Sblock), SEEK_SET);
    fwrite(&inodos, sizeof(inodos), 1, dsk);

    for (int i = 0; i < 3 * n; i++)
    {
        bloques[i] = '0';
    }
    bloques[0] = '1';
    bloques[1] = '1';

    fseek(dsk, PosSupB + sizeof(Sblock) + sizeof(inodos), SEEK_SET);
    fwrite(&bloques, sizeof(bloques), 1, dsk);

    I_node inodoRoot;
    inodoRoot.i_uid = 1;
    inodoRoot.i_gid = 1;
    inodoRoot.i_size = 0;
    strcpy(inodoRoot.i_atime, fechayhora);
    strcpy(inodoRoot.i_ctime, fechayhora);
    strcpy(inodoRoot.i_mtime, fechayhora);
    inodoRoot.i_perm = 664;
    inodoRoot.i_type = '0';
    inodoRoot.i_block[0] = 0;

    for (int i = 1; i < 15; i++)
    {
        inodoRoot.i_block[i] = -1;
    }

    BloqueCarpetas bloqueCarpeta1;

    bloqueCarpeta1.b_content[0].b_inodo = 0;
    strcpy(bloqueCarpeta1.b_content[0].b_name, ".");

    bloqueCarpeta1.b_content[1].b_inodo = 0;
    strcpy(bloqueCarpeta1.b_content[1].b_name, "..");

    bloqueCarpeta1.b_content[2].b_inodo = 1;
    strcpy(bloqueCarpeta1.b_content[2].b_name, "users.txt");

    bloqueCarpeta1.b_content[3].b_inodo = -1;
    strcpy(bloqueCarpeta1.b_content[3].b_name, "");


    string contenido = "1,G,root\n1,U,root,root,123\n";
    I_node inodoArchivo;
    inodoArchivo.i_uid = 1;
    inodoArchivo.i_gid = 1;
    inodoArchivo.i_size = sizeof(contenido);
    strcpy(inodoArchivo.i_atime, fechayhora);
    strcpy(inodoArchivo.i_ctime, fechayhora);
    strcpy(inodoArchivo.i_mtime, fechayhora);
    inodoArchivo.i_perm = 664;
    inodoArchivo.i_type = '1';
    inodoArchivo.i_block[0] = 1;
    for (int i = 1; i < 15; i++)
    {
        inodoArchivo.i_block[i] = -1;
    }

    BloqueArchivos bl_archivo;
    strcpy(bl_archivo.b_content, contenido.c_str());

    //Escribo el inodo Raiz
    fseek(dsk, SupBlock.s_inode_start, SEEK_SET);
    fwrite(&inodoRoot, sizeof(I_node), 1, dsk);

    //Escribo el bloque de carpetas de la raiz
    fseek(dsk, SupBlock.s_block_start, SEEK_SET);
    fwrite(&bloqueCarpeta1, sizeof(BloqueCarpetas), 1, dsk);

    //Escribo el inodo del archivo
    fseek(dsk, SupBlock.s_inode_start + sizeof(I_node), SEEK_SET);
    fwrite(&inodoArchivo, sizeof(I_node), 1, dsk);

    //Escribo bloque del archivo
    fseek(dsk, SupBlock.s_block_start + sizeof(BloqueCarpetas), SEEK_SET);
    fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);

    cout << "Particion formateada en EXT2" << endl;

    fclose(dsk);

};

void Mkfs::CreateExt3(Mounter Particion){
    
    double n = 0;
    string path;
    path = Particion.path;
    int Estructs_Num;
    time_t t;
    struct tm *tm;
    char fechayhora[20];
    char nulo = '0';

    t = time(NULL);
    tm = localtime(&t);
    strftime(fechayhora, 20, "%d/%m/%Y %H:%M:%S", tm);
    int startpoint = Particion.startpoint;

    n = (Particion.size - sizeof(Sblock)) / (4 + sizeof(Journal) + sizeof(I_node) + 3*(sizeof(BloqueArchivos)));
    n = (Particion.size - sizeof(Sblock)) / (4 + sizeof(I_node) + 3*sizeof(BloqueArchivos));

    Estructs_Num = floor(n);

    if (n < 1){
        cout << "El tamaño de la partición es muy pequeño para contener el sistema de archivos" << endl;
        return;
    }

    Estructs_Num = int(floor(n));
    int num_block = 3*Estructs_Num;

    FILE* dsk = fopen(path.c_str(), "rb+");

    Sblock SupBlock;

    SupBlock.s_filesystem_type = 3;
    SupBlock.s_inodes_count = Estructs_Num;
    SupBlock.s_blocks_count = num_block;
    SupBlock.s_free_inodes_count = Estructs_Num-2;
    SupBlock.s_free_blocks_count = num_block-2;
    strcpy(SupBlock.s_mtime, fechayhora);
    strcpy(SupBlock.s_umtime, fechayhora);
    SupBlock.s_mnt_count = 1;
    SupBlock.s_magic = 0xEF53;
    SupBlock.s_inode_size = sizeof(I_node);
    SupBlock.s_block_size = 64;
    SupBlock.s_firts_ino = 3;
    SupBlock.s_first_blo = 3;

    int PosSupB = startpoint;
    fseek(dsk, startpoint, SEEK_SET);
    if(Particion.type == 'e'){
        PosSupB = startpoint + sizeof(EBR);
        SupBlock.s_block_start = startpoint + sizeof(EBR) + int(sizeof(Sblock)) + Estructs_Num + num_block + (int(sizeof(I_node))*Estructs_Num);
        SupBlock.s_bm_inode_start = startpoint + int(sizeof(Sblock)) + n * sizeof(Journal)+ sizeof(EBR);
        SupBlock.s_bm_block_start = startpoint + int(sizeof(Sblock)) + Estructs_Num+ sizeof(EBR);
        SupBlock.s_inode_start = startpoint + int(sizeof(Sblock)) + Estructs_Num + num_block+ sizeof(EBR);

        for (int i = 0; i < Particion.size - sizeof(EBR); i++){
            fwrite(&nulo, sizeof(nulo), 1, dsk);
            fseek(dsk, PosSupB + i, SEEK_SET);
        }

        fseek(dsk, PosSupB, SEEK_SET);
    }else {

        SupBlock.s_block_start = startpoint + int(sizeof(Sblock)) + Estructs_Num + num_block + (int(sizeof(I_node))*Estructs_Num);
        SupBlock.s_bm_inode_start = startpoint + int(sizeof(Sblock)) + n * sizeof(Journal);
        SupBlock.s_bm_block_start = startpoint + int(sizeof(Sblock)) + Estructs_Num;
        SupBlock.s_inode_start = startpoint + int(sizeof(Sblock)) + Estructs_Num + num_block;

        for (int i = 0; i < Particion.size; i++){
            fwrite(&nulo, sizeof(nulo), 1, dsk);
            fseek(dsk, PosSupB + i, SEEK_SET);
        }

        fseek(dsk, PosSupB, SEEK_SET);
    }
    
    fwrite(&SupBlock, sizeof(Sblock), 1, dsk);
    
    Journal journal;
    journal.estado = 0;
    journal.fecha_op[0] = '\0';
    journal.path[0] = '\0';
    journal.type = -1;
    strcpy(journal.contenido, "--");
    journal.tam = 0;
    journal.tipo_op[0] = '\0';

    //journal de la carpeta
    Journal journalCarpeta;
    journalCarpeta.estado = 1;
    strcpy(journalCarpeta.tipo_op, "mkfs");
    strcpy(journalCarpeta.fecha_op, fechayhora);
    strcpy(journalCarpeta.path, "/");
    journalCarpeta.id_propietario = '1';
    strcpy(journalCarpeta.contenido, "-");
    journalCarpeta.type = '0';

    //journal del archivo
    Journal journalArchivo;
    journalArchivo.estado = 1;
    strcpy(journalArchivo.tipo_op, "mkfs");
    strcpy(journalArchivo.fecha_op, fechayhora);
    strcpy(journalArchivo.path, "/users.txt");
    journalArchivo.id_propietario = '1';
    strcpy(journalArchivo.contenido, "1,G,root\n1,U,root,root,123\n");
    journalArchivo.type = '1';


    for (int i = 0; i < n; i++){
        if (i == 0){
            fseek(dsk, PosSupB+sizeof(Sblock), SEEK_SET);
            fwrite(&journalCarpeta, sizeof(Journal), 1, dsk);
        }
        else if (i == 1){
            fseek(dsk, PosSupB + sizeof(Sblock) + sizeof(Journal), SEEK_SET);
            fwrite(&journalArchivo, sizeof(Journal), 1, dsk);
        }
        else if (i > 1){
            fseek(dsk, PosSupB + sizeof(Sblock) + i * sizeof(Journal), SEEK_SET);
            fwrite(&journal, sizeof(Journal), 1, dsk);
        }
    }

    char inodos[int(n)];
    char bloques[3 * int(n)];

    for (int i = 0; i < n; i++)
    {
        inodos[i] = '0';
    }

    inodos[0] = '1';
    inodos[1] = '1';
    fseek(dsk, SupBlock.s_bm_inode_start, SEEK_SET);
    fwrite(&inodos, sizeof(inodos), 1, dsk);

     for (int i = 0; i < 3 * n; i++)
    {
        bloques[i] = '0';
    }

    bloques[0] = '1';
    bloques[1] = '1';

    fseek(dsk, SupBlock.s_bm_block_start, SEEK_SET);
    fwrite(&bloques, sizeof(bloques), 1, dsk);

    I_node inodoRoot;
    inodoRoot.i_uid = 1;
    inodoRoot.i_gid = 1;
    inodoRoot.i_size = 0;
    strcpy(inodoRoot.i_atime, fechayhora);
    strcpy(inodoRoot.i_ctime, fechayhora);
    strcpy(inodoRoot.i_mtime, fechayhora);
    inodoRoot.i_perm = 664;
    inodoRoot.i_type = 0;
    inodoRoot.i_block[0] = 0;
    for (int i = 1; i < 15; i++){
        inodoRoot.i_block[i] = -1;
    }


    BloqueCarpetas bloqueCarpeta1;

    bloqueCarpeta1.b_content[0].b_inodo = 0;
    strcpy(bloqueCarpeta1.b_content[0].b_name, ".");

    bloqueCarpeta1.b_content[1].b_inodo = 0;
    strcpy(bloqueCarpeta1.b_content[1].b_name, "..");

    bloqueCarpeta1.b_content[2].b_inodo = 1;
    strcpy(bloqueCarpeta1.b_content[2].b_name, "users.txt");

    bloqueCarpeta1.b_content[3].b_inodo = -1;
    strcpy(bloqueCarpeta1.b_content[3].b_name, "");

    string contenido = "1,G,root\n1,U,root,root,123\n";
    I_node inodoArchivo;
    inodoArchivo.i_uid = 1;
    inodoArchivo.i_gid = 1;
    inodoArchivo.i_size = sizeof(contenido);
    strcpy(inodoArchivo.i_atime, fechayhora);
    strcpy(inodoArchivo.i_ctime, fechayhora);
    strcpy(inodoArchivo.i_mtime, fechayhora);
    inodoArchivo.i_perm = 664;
    inodoArchivo.i_type = '1';
    inodoArchivo.i_block[0] = 1;
    for (int i = 1; i < 15; i++)
    {
        inodoArchivo.i_block[i] = -1;
    }


    BloqueArchivos bl_archivo;
    strcpy(bl_archivo.b_content, contenido.c_str());

    //Escribo el inodo Raiz
    fseek(dsk, SupBlock.s_inode_start, SEEK_SET);
    fwrite(&inodoRoot, sizeof(I_node), 1, dsk);

    //Escribo el bloque de carpetas de la raiz
    fseek(dsk, SupBlock.s_block_start, SEEK_SET);
    fwrite(&bloqueCarpeta1, sizeof(BloqueCarpetas), 1, dsk);

    //Escribo el inodo del archivo
    fseek(dsk, SupBlock.s_inode_start + sizeof(I_node), SEEK_SET);
    fwrite(&inodoArchivo, sizeof(I_node), 1, dsk);

    //Escribo bloque del archivo
    fseek(dsk, SupBlock.s_block_start + sizeof(BloqueCarpetas), SEEK_SET);
    fwrite(&bl_archivo, sizeof(BloqueArchivos), 1, dsk);

    cout << "Particion formateada en EXT3" << endl;

    fclose(dsk);

};