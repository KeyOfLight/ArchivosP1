#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <fstream>
#include "./Analizar/Analizador.cpp"
#include "Estructuras.cpp"
#include <vector>

using namespace std;

void menu();
void ingresarcomando();

Ann analisis;
vector<Mounter> mounted;

int main(){

    int opcion;

    while (opcion != 0){
        cout << "***********MENU GENERAL**********" << endl
        << "*****Diego Andre Gomez 201908327*****" << endl
        << "1) Ingresar comando*" << endl
        << "0) Salir de la aplicacion" << endl
        << "*******************************" << endl;

        cin >> opcion; 

        switch (opcion)
        {
        case 1:
            ingresarcomando();
            break;
        
        default:
            cout << "Ingrese una opcion valida"<<endl;
        }
    } 
    
}
//Recuerda terminar el worstF

void ingresarcomando(){

    string useramne; //Comando que se desea ejecutar-- Me acabo de dar cuenta que no tome en cuenta los apuntadores indirecto ni doble indirectos al momento de moverme en las carpetas para buscar archivos WIP
    cout << "Escriba el comando que desea ejecutar" << endl;

    cin.ignore(numeric_limits<int>::max(),'\n');
    getline(cin,useramne);
    
    analisis.Analizar(useramne);
}