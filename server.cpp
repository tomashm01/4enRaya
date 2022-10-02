#include "models/Server.cpp"
#include "models/Game.cpp"
#include <signal.h>
using namespace std;

#define MAX_CLIENTS 30
#define PORT 2064
#define MAX_GAMES 10
#define FILE "users.txt"

bool loginUser(string username, string password)
{
	string line;
	string data = username + "," + password;
	ifstream myfile(FILE);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if (strcmp(line.c_str(), data.c_str()) == 0)
			{
				return true;
			}
		}
		myfile.close();
	}
	return false;
}

bool registerUser(string username, string password)
{
	if (!loginUser(username, password))
	{
		ofstream myfile;
		myfile.open(FILE, ios::app);
		myfile <<'\n'<< username + "," + password;
		myfile.close();
		return true;
	}
	return false;
}

bool existsUser(string username)
{
	string line;
	ifstream myfile(FILE);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if (strcmp(line.substr(0, line.find(',')).c_str(), username.c_str()) == 0)
			{
				return true;
			}
		}
		myfile.close();
	}
	return false;
}

void manejador (int signum){
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT,manejador);
	exit(0);
}

vector<string> split(string str, char pattern) {
    
    int posInit = 0;
    int posFound = 0;
    string splitted;
    vector<string> results;
    
    while(posFound >= 0){
        posFound = str.find(pattern, posInit);
        splitted = str.substr(posInit, posFound - posInit);
        posInit = posFound + 1;
        results.push_back(splitted);
    }
    
    return results;
}

int main(){
	Server server;
	Game game;
	string user,password;
	bool logged = false;
	//Maximo hay 30 clientes en cola de espera y se abre en puerto 2060
	server.bindListen(PORT, MAX_CLIENTS);
	signal(SIGINT,manejador);

	while (true)
	{
		server.acceptRequest();
		do{ 
			server.freeBuffer();
			server.receiveRequest();
			string buffer(server.getBuffer());
			vector<string> data = split(buffer, ' ');
			cout<<data[0]<<data[1]<<endl;
			
			if(strcmp(data[0].c_str(),"USUARIO")==0 && data.size()==2){ //Inicio sesión
			
				if(existsUser(data[1])){ //USUARIO user
					
					user=data[1];
					server.sendRequest("+Ok. Usuario correcto\n");
					server.receiveRequest();
					buffer=server.getBuffer();
					data=split(buffer, ' ');
					server.freeBuffer();

					if(strcmp(data[0].c_str(),"PASSWORD")==0 && data.size()==2){ //PASSWORD pass
						password=data[1];
						if(loginUser(user,password)){
							server.sendRequest("+Ok. Usuario validado\n");
						}else{
							server.sendRequest("-Err. Error en la validación\n");
							user="";
							password="";
						}
					}
				}else{
					server.sendRequest("-Err. Usuario incorrecto\n");
					user="";
					password="";
				}
			} else if(strcmp(data[0].c_str(),"REGISTRO")==0 && data.size()==5){ // REGISTRO -u user -p pass
				user=data[2];
				password=data[4];
				server.freeBuffer();
				if(strcmp(data[1].c_str(),"-u")==0 && strcmp(data[3].c_str(),"-p")==0){
					if(registerUser(user,password)){
						server.sendRequest("+Ok. Usuario registrado\n");
					}else{
						server.sendRequest("-Err. Error en el registro\n");
					}
				}
			}else{//Ninguna de las opciones anteriores
				server.sendRequest("-Err. Elige una opcion\n");
				server.freeBuffer();
			}

		} while (strcmp(server.getBuffer(), "SALIR") != 0);
		cout << "Cerrando conexion" << endl;
		server.closeNewSD();
		server.closeSD();
	}

	return 0;
}
