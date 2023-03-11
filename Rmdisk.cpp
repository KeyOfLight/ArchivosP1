#include <iostream>

using namespace std;


class Rmdisk{
    public:

    int Remove(string Dir);
    bool existeDisco(string path);
};

bool Rmdisk::existeDisco(string path) { //Revisa si existe un disco para poder llevar a cabo la elminiacion del mismo
    FILE *disk = fopen(path.c_str(), "rb");

    if (!disk) {
        return false;
    }

    fclose(disk);
    return true;
}

int Rmdisk::Remove(string Dir){ //Se encarga de eliminar el disco proporcionado por el usuario

    string Path = Dir;

    if(!existeDisco(Path)){
        cout << "El disco no se pudo encontrar, no se puede borrar"<< endl;
        return 0;
    }

    string opcion;
    bool Confirmar = false;

    while (!Confirmar) {
        cout << "Esta seguro que desea borrar el disco " + Path + "?" << endl;
        cout << "1) Si"<<endl;
        cout << "2) No"<< endl;

        cin >> opcion;

        if(opcion == "1"){
            Confirmar = true;
            cout << "Se confirmo que se desea eliminar el disco " + Path + ", se procedera a elminarlo" << endl;
            break;
        }else if(opcion == "2"){
            Confirmar = true;
            cout << "Se cancelara la eliminacion del disco"<< endl;
            return 0;
        }else {
            cout<<"Ingrese una opcion valdia"<<endl;
        }
            
        
    }
    

    string comando1 = "sudo rm \"" + Path + "\"";
    system(comando1.c_str());

    cout << endl << "Disco Eliminado Exitosamente" << endl << endl;

    return 1;

}   