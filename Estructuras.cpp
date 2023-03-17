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
    string Id;

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
    int i_uid; // UID del usuario propietario del archivo o carpeta
    int i_gid; // GID del grupo al que pertenece el archivo o carpeta.
    int i_size; // Tamaño del archivo en bytes
    char i_atime[20]; // Última fecha en que se leyó el inodo sin modificarlo
    char i_ctime[20]; // Fecha en la que se creó el inodo
    char i_mtime[20]; // Última fecha en la que se modifica el inodo
    int i_block[15]; // Array en los que los primeros 12 registros son bloques directos.
    char i_type; // Indica si es archivo o carpeta. Tendrá los siguientes valores: 1 Archivo, 0 Carpeta
    int i_perm; // Guardará los permisos del archivo o carpeta.
};

struct Sblock{

    int s_filesystem_type; //Guarda el número que identifica el sistema de archivos utilizado
    int s_inodes_count; //Guarda el número total de inodos
    int s_blocks_count; //Guarda el número total de bloques
    int s_free_blocks_count; // Contiene el número de bloques libres
    int s_free_inodes_count; // Contiene el número de inodos libres
    char s_mtime[20]; // Última fecha en el que el sistema fue montado
    char s_umtime[20]; // Última fecha en que el sistema fue desmontado
    int s_mnt_count ; // Indica cuantas veces se ha montado el sistema
    int s_magic; // Valor que identifica al sistema de archivos, tendrá el valor 0xEF53
    int s_inode_size; // Tam inodo
    int s_block_size; // Tam bloque
    int s_firts_ino; //Primer inodo libre
    int s_first_blo; // Primer bloque libre
    int s_bm_inode_start; // Guardará el inicio del bitmap de inodos
    int s_bm_block_start; // Guardará el inicio del bitmap de bloques
    int s_inode_start; // Guardará el inicio de la tabla de inodos
    int s_block_start; // Guardará el inicio de la tabla de bloques
};

struct Content{
    char b_name[12]; // Nombre de la carpeta o archivo
    int b_inodo; // Apuntador hacia un inodo asociado al archivo o carpeta
};

struct BloqueCarpetas{
    Content b_content[4]; // Array con el contenido de la carpeta
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
    char b_content[64]; // Array con el contenido del archivo
};

struct BloqueApuntadores{
    int b_pointers[16]; // Array con los apuntadores a bloques (de archivo o carpeta)
};

struct Grupo {
    char GID;
    char Type;
    char Grupo[11];
};

struct User {
    char UID;
    char Type;
    char Grupo[11];
    char Pass[11];
    char Uss[11];
    string path;
    int startpoint;
};


class Comando //Clase que tiene toda la informacion para guardar un comando
{
public:
    string nombre = "";
    Parametros param;
};

#endif
