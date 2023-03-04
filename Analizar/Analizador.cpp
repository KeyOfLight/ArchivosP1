#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include<bits/stdc++.h>
#include "../Estructuras.cpp"
#include "../Comandos.cpp"
#include "../Reportes.cpp"
#include "../Rmdisk.cpp"
#include "../Fdisk.cpp"

using namespace std;

class Ann{
public:
    void Analizar(string Cadena);
    vector<string> split_txt(string text);
    void ReconocerComando(string comando, vector<string> parametros);
    string replace_txt(string str, const string& from, const string& to);
    void Execute(string dir);
    //
    Comando cmd;
    Comandos procesar;
    Reportes report;
    Rmdisk remover;
    Fdisk Part;

    vector<Mounter> mounted;
};

struct TiposDeComandos  //Listado de comandos que se utilizan en el proyecto
{
    string Execute = "Execute";
    string mkdisk = "MkDisk";
    string rep = "Rep";
    string Remove = "RMDisk";
    string fdisk = "FDisk";
};


//Separa la cadena que se le manda desde el menu
void Ann::Analizar(string Cadena){
    string entradacmd = Cadena;
    transform(entradacmd.begin(), entradacmd.end(), entradacmd.begin(), ::tolower);

    vector<string> VectorEntrada = split_txt(entradacmd); //Se separan los comandos
    vector<string> parametros;                            //Se guardan los parametros de cada comando
    string comando = "";
    for(int i = 0; i< VectorEntrada.size(); i++){
        if(i == 0){
            comando = VectorEntrada.at(i);
        }else{
            parametros.push_back(VectorEntrada.at(i));
        }
    }

    ReconocerComando(comando, parametros);
}



//Lee los comandos para poder clasificarlos
void Ann::ReconocerComando(string comando, vector<string> parametros){
    string param = "";
    TiposDeComandos ComandosNombre;
    string separator = "";
    string ComandoSeparado;
    Parametros emptParam;
    stringstream ss(comando);

    while(getline(ss, comando, ' ')){
        ComandoSeparado = comando;
        }

    if(ComandoSeparado == "execute"){///////////////////execute
        cmd.nombre = ComandosNombre.Execute;
        if (parametros.size() == 0){
            cout<< "Debe ingresar un path para poder ejecutar este comando" << endl;
            return;
        }else{
        for (int i = 0; i < parametros.size(); i++){
                param = parametros.at(i);
                if(param.find(">path=") == 0){
                    param = replace_txt(param, ">path=", "");
                    cmd.param.direccion = param;
                }
            }
            Execute(cmd.param.direccion);
            //Ejecutar el comando
        }
    }else if(ComandoSeparado == "mkdisk"){      ////////MKDisk
        cmd.nombre = ComandosNombre.mkdisk;

        for (int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">size") == 0){
                param = replace_txt(param, ">size=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.tam = param;
            }else if(param.find(">path") == 0){
                param = replace_txt(param, ">path=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.direccion = param;
            }else if(param.find(">fit") == 0){
                param = replace_txt(param, ">fit=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.fit = param;
            }else if(param.find(">unit") == 0){
                param = replace_txt(param, ">unit=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.unit = param;
            }
        }

        procesar.MkDisk(cmd.param);//Ejecuta la creacion del disco

    }else if(ComandoSeparado == "rmdisk"){//////////////////////// Rmdisk
        cmd.nombre = ComandosNombre.mkdisk;

        for (int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">path") == 0){
                param = replace_txt(param, ">path=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.direccion = param;
            }
        }
        remover.Remove(cmd.param.direccion);    ///////////////////Ejecutar Remove
    }else if(ComandoSeparado == "fdisk"){   /////////////////////////Fdisk
        cmd.nombre = ComandosNombre.fdisk;
        for (int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">size") == 0){
                    param = replace_txt(param, ">size=", "");
                    param = replace_txt(param, "\"", "");
                    cmd.param.tam = param;
            }else if(param.find(">path") == 0){
                    param = replace_txt(param, ">path=", "");
                    param = replace_txt(param, "\"", "");
                    cmd.param.direccion = param;
            }else if(param.find(">name") == 0){
                    param = replace_txt(param, ">name=", "");
                    param = replace_txt(param, "\"", "");
                    cmd.param.nombre = param;
            }else if(param.find(">unit") == 0){
                    param = replace_txt(param, ">unit=", "");
                    param = replace_txt(param, "\"", "");
                    cmd.param.unit = param;
            }else if(param.find(">type") == 0){
                    param = replace_txt(param, ">type=", "");
                    param = replace_txt(param, "\"", "");
                    cmd.param.type = param;
            }else if(param.find(">fit") == 0){
                    param = replace_txt(param, ">fit=", "");
                    param = replace_txt(param, "\"", "");
                    cmd.param.fit = param;
            }else if(param.find(">delete") == 0){
                    param = replace_txt(param, ">delete=", "");
                    param = replace_txt(param, "\"", "");
                    cmd.param.del = param;
            }else if(param.find(">add") == 0){
                    param = replace_txt(param, ">add=", "");
                    param = replace_txt(param, "\"", "");
                    cmd.param.add = param;
            }
        }
        Part.CrearParticion(cmd.param);
    }else if(ComandoSeparado == "rep"){/////////////////////////////////// Rep
        cmd.nombre = ComandosNombre.rep;
        for (int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">name") == 0){
                    param = replace_txt(param, ">name=", "");
                    param = replace_txt(param, "\"", "");
                    cmd.param.nombre = param;
            }else if(param.find(">path") == 0){
                    param = replace_txt(param, ">path=", "");
                    param = replace_txt(param, "\"", "");
                    cmd.param.direccion = param;
            }
        }
        report.GenerarReporte(cmd.param.direccion); //Ejecuta Generar Reporte

    }

    cmd.param = emptParam;
}

void Ann::Execute(string dir){

    fstream Archivo; 
    vector<string> Comandos;
    string linea;


    Archivo.open(dir, ios::in);

    if (!Archivo.is_open()) {
        cout << "no se pudo abrir el archivo deseado" << endl;
        return;
    }

    while (getline(Archivo, linea)) { 
            Comandos.push_back(linea);
        }

    for (const auto &i : Comandos){
        Analizar(i);
    }

    Archivo.close();

}  

vector<string> Ann::split_txt(string text){ // Split para separar tipo de comando y parametros
    stringstream text_to_split(text);
    string segment;
    vector<string> splited;
    
    while(getline(text_to_split, segment,' ')){
       splited.push_back(segment);
    }
    return splited;

}


string Ann::replace_txt(string str, const string& from, const string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}



