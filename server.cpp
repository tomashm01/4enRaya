#include "models/Server.cpp"
#include <signal.h>
using namespace std;

#define MAX_CLIENTS 30
#define PORT 2063
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

void manejador (int signum){
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT,manejador);
	exit(0);
}

int main()
{
	Server server;
	// Maximo hay 30 clientes en cola de espera y se abre en puerto 2060
	server.bindListen(PORT, MAX_CLIENTS);
	signal(SIGINT,manejador);

	while (true)
	{
		server.acceptRequest();
		do
		{
			server.freeBuffer();
			server.receiveRequest();
			cout << "Mensaje recibido: " << server.getBuffer() << endl;
		} while (strcmp(server.getBuffer(), "0") != 0);
		cout << "Cerrando conexion" << endl;
		server.closeNewSD();
		server.closeSD();
	}

	return 0;
}
