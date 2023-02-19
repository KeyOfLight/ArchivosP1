#ifndef Estructuras_cpp
#define Estructuras_cpp

#include <string>
#include <iostream>

using namespace std;



struct Parametros //Parametros de todos los comandos que se van a utilizar
{
    string direccion;
    string tam;
    string unit;
    string fit;
    string nombre;

};

struct Particion {
    char    status = '0';
    char    type = ' ';
    char    fit = ' ';
    int     start = 0;
    int     size = 0;
    char    name[16] = "";
};

struct MBR {
    int         tamano;
    char        fecha_creacion[20];
    int         disk_signature;
    Particion   particiones[4];
};

class Comando //Clase que tiene toda la informacion para guardar un comando
{
public:
    string nombre = "";
    Parametros param;
};

#endif
