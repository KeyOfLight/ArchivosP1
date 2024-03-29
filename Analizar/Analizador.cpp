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
#include "../Mount.cpp"
#include "../Mkfs.cpp"
#include "../Login.cpp"
#include "../Mkgrp.cpp"

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
    Mount Montador;
    Mkfs MakeFileS;
    Login lgn;
    Mkgrp Makegrp;

    vector<Mounter> mounted;
    User Logued;
    User empt;
};

struct TiposDeComandos  //Listado de comandos que se utilizan en el proyecto
{
    string Execute = "Execute";
    string mkdisk = "MkDisk";
    string rep = "Rep";
    string Remove = "RMDisk";
    string fdisk = "FDisk";
    string Mount = "Mount";
    string Mkfs = "Mkfs";
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
    }else if(ComandoSeparado == "mount"){//////////////////////// Mount
        cmd.nombre = ComandosNombre.Mount;
        for (int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">path") == 0){
                param = replace_txt(param, ">path=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.direccion = param;
            }else if(param.find(">name") == 0){
                param = replace_txt(param, ">name=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.nombre = param;
            }
        }
        Mounter tempmount;
        tempmount = Montador.Montar(cmd.param.direccion, cmd.param.nombre);    ///////////////////Ejecutar Montar
        if(tempmount.id != "")
            mounted.push_back(tempmount);

    }else if(ComandoSeparado == "unmount"){//////////////////////// Unmount
        cmd.nombre = ComandosNombre.Mount;
        for (int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">id") == 0){
                param = replace_txt(param, ">id=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.nombre = param;
            }
        }
        mounted = (Montador.Unmount(cmd.param.nombre, mounted));    ///////////////////Ejecutar Montar

    }else if(ComandoSeparado == "mkfs"){//////////////////////// Mkfs
        cmd.nombre = ComandosNombre.Mkfs;
        for (int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">id") == 0){
                param = replace_txt(param, ">id=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.nombre = param;
            }
            if(param.find(">type") == 0){
                param = replace_txt(param, ">type=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.type = param;
            }
            if(param.find(">fs") == 0){
                param = replace_txt(param, ">fs=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.fs = param;
            }
        }
        MakeFileS.StartMkfs(mounted, cmd.param);    ///////////////////Ejecutar Make file system

    }else if(ComandoSeparado == "login"){//////////////////////// Login
        cmd.nombre = ComandosNombre.Mount;
        for (int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">id") == 0){
                param = replace_txt(param, ">id=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.nombre = param;
            }
            if(param.find(">user") == 0){
                param = replace_txt(param, ">user=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.user = param;
            }
            if(param.find(">pass") == 0){
                param = replace_txt(param, ">pass=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.pass = param;
            }
        }
        Logued = lgn.Loguear(mounted, cmd.param);    ///////////////////Ejecutar Login

    }else if(ComandoSeparado == "logout"){//////////////////////// Logout
        
        if(Logued.startpoint != 0){
            Logued = empt;
        }else{
            cout<< "No se puede desloguear debido a que no esta logueado en ninguna sesion"<< endl;
        }

    }else if(ComandoSeparado == "mkgrp"){//////////////////////// Mkgrp
        
        cmd.nombre = ComandosNombre.Mkfs;
        for (int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">name") == 0){
                param = replace_txt(param, ">name=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.nombre = param;
            }
        }
        Makegrp.CreateGrp(Logued,cmd.param);

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
            }else if(param.find(">id") == 0){
                    param = replace_txt(param, ">id=", "");
                    param = replace_txt(param, "\"", "");
                    cmd.param.Id = param;
            }else if(param.find(">ruta") == 0){
                    param = replace_txt(param, ">ruta=", "");
                    param = replace_txt(param, "\"", "");
                    cmd.param.pass = param;
            }
        }
        report.GenerarReporte(cmd.param, Logued); //Ejecuta Generar Reporte

    }else if(ComandoSeparado.find("#") == 0){//////////////////////// Comentario
        string imprimir = ComandoSeparado;
        for (int i = 0; i < parametros.size(); i++){
            param = parametros.at(i);
            imprimir += " "+param;
        }
        cout<<imprimir<<endl;
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
    string temp;


    
    while(getline(text_to_split, segment,' ')){
        if(segment.find(">path=\"")==0){
            temp = segment;
            getline(text_to_split, segment,' ');
            temp += " "+segment;
            splited.push_back(temp);
        }else{
            splited.push_back(segment);
        }
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



