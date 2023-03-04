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
};

class Comando //Clase que tiene toda la informacion para guardar un comando
{
public:
    string nombre = "";
    Parametros param;
};

#endif
