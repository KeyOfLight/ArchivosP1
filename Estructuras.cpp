#ifndef Estructuras_cpp
#define Estructuras_cpp

#include <string>
#include <iostream>
#include <vector>

using namespace std;



struct Parametros //Parametros de todos los comandos que se van a utilizar
{
    string direccion;
    string tam;
    string unit;
    string fit;
    string nombre;
    string status;
    string type;
    string start;
    string size ;
    string del;
    string add;
    string fs;
    string user;
    string pass;

};

struct Particion {
    
    char    status = '0'; //Indica si la particion se encuentra activa (1) o inactiva (0)
    char    type = ' ';   //Indica el tipo de partición, primaria o extendida. Tendrlos valores P o E
    char    fit = ' ';      //Indica el tipo de ajuste que se utiliza (B,F,W)
    int     start = 0;      //Indica la posicion donde empiza la particion
    int     size = 0;       //Indica el tam;ao de la particion
    char    name[16] = "";  //indica el nobmre de la particion
};

struct EBR {
    char    status = '0'; //Indica si la particion se encuentra activa (1) o inactiva (0)
    char    fit = ' ';      //Indica el tipo de ajuste que se utiliza (B,F,W)
    int     start = 0;      //Indica la posicion donde empiza la particion
    int     size = 0;       //Indica el tam;ao de la particion
    int     p_siguiente = 0;    // Indica el byte en el que esta la proxima particion
    char    name[16] = "";  //indica el nobmre de la particion
    
};

struct MBR {
    int         tamano;
    char        fecha_creacion[20];
    int         disk_signature;
    char        disk_fit;
    Particion   particiones[4];
};

struct Fits {
    vector <string> FitType = {"bf","ff","wf"};
};

struct Mounter {
    string id;
    string name;
    string path;
    int startpoint = 0;
    int size = 0;
    char type = ' ';
};

struct I_node {
    int i_uid;
    int i_gid;
    int i_size;
    char i_atime[20];
    char i_ctime[20];
    char i_mtime[20];
    int i_block[15];
    char i_type;
    int i_perm;
};

struct Sblock{

    int s_filesystem_type;
    int s_inodes_count;
    int s_blocks_count;
    int s_free_blocks_count;
    int s_free_inodes_count;
    char s_mtime[20];
    char s_umtime[20];
    int s_mnt_count ;
    int s_magic;
    int s_inode_size;
    int s_block_size;
    int s_firts_ino;
    int s_first_blo;
    int s_bm_inode_start;
    int s_bm_block_start;
    int s_inode_start;
    int s_block_start;
};

struct Content{
    char b_name[12];
    int b_inodo;
};

struct BloqueCarpetas{
    Content b_content[4];
};

struct Journal{
    int estado;
    char tipo_op[10];
    int type;
    char path[100];
    char fecha_op[25];
    char contenido[100];
    char id_propietario;
    int tam; 

};

struct BloqueArchivos{
    char b_content[64];
};

struct BloqueApuntadores{
    int b_pointers[16];
};

struct Grupo {
    int GID;
    int Type;
    char Grupo[10];
};

struct User {
    int UID;
    int Type;
    char Grupo[10];
    char Pass[10];
    char Uss[10];
};


struct credentials{
    string pass;
    string usser;
    bool logueado = false;
};

class Comando //Clase que tiene toda la informacion para guardar un comando
{
public:
    string nombre = "";
    Parametros param;
};

#endif
