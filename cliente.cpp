#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;
#define PORT 2060
#define IP "127.0.0.1"

void mostrarMenuPartida()
{
	vector<string> opciones = {
		"COLOCAR-FICHA número",
		"SALIR"};
	for (int i = 0; i < opciones.size(); i++)
	{
		cout << opciones[i] << endl;
	}
	cout << endl;
}

void mostrarMenuEspera()
{
	vector<string> opciones = {"SALIR"};
	for (int i = 0; i < opciones.size(); i++)
	{
		cout << opciones[i] << endl;
	}
	cout << endl;
}

void mostrarOpciones()
{
	vector<string> opciones = {
		"USUARIO usuario",
		"PASSWORD password",
		"REGISTRO -u usuario -p password",
		"INICIAR-PARTIDA",
		"COLOCAR-FICHA número",
		"SALIR"};
	for (int i = 0; i < opciones.size(); i++)
	{
		cout << opciones[i] << endl;
	}
	cout << endl;
}

vector<string> split(string str, char pattern)
{

	int posInit = 0;
	int posFound = 0;
	string splitted;
	vector<string> results;

	while (posFound >= 0)
	{
		posFound = str.find(pattern, posInit);
		splitted = str.substr(posInit, posFound - posInit);
		posInit = posFound + 1;
		results.push_back(splitted);
	}

	return results;
}

int openSocket()
{
	int sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
		exit(1);
	}
	return sd;
}

int main()
{
	int sd;
	struct sockaddr_in sockname;
	char buffer[250];
	int tamTablero = 0;
	char tablero[250];
	socklen_t len_sockname;
	fd_set readfds, auxfds;
	int salida;
	int fin = 0;

	sd = openSocket();

	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(PORT);
	sockname.sin_addr.s_addr = inet_addr(IP);
	FD_ZERO(&auxfds);
	FD_ZERO(&readfds);
	FD_SET(0, &readfds);
	FD_SET(sd, &readfds);
	len_sockname = sizeof(sockname);

	if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1)
	{
		perror("Error de conexión");
		exit(1);
	}

	do
	{
		auxfds = readfds;
		salida = select(sd + 1, &auxfds, NULL, NULL, NULL);

		if (FD_ISSET(sd, &auxfds))
		{

			bzero(buffer, sizeof(buffer));
			recv(sd, buffer, sizeof(buffer), 0);
			

			// Comprobar que el buffer empieza por "ESPERANDO"
			string bufferStr(buffer);
			vector<string> bufferSplit = split(bufferStr, '.');


			if (bufferSplit.size()>1 && strcmp(bufferSplit[1].c_str(), "Nuevo tablero") == 0)
			{
				cout << "|1|2|3|4|5|6|7|" << endl
					 << "|";
				int contador = 0;
				for (int i = 0; i < sizeof(buffer); i++)
				{

					if (buffer[i] == ';')
					{
						cout << "|" << endl;
						contador++;
						if (contador != 6)
						{
							cout << "|";
						}
					}
					else if (buffer[i] == '-')
					{
						cout << " ";
					}
					else if (buffer[i] == ',')
					{
						cout << "|";
					}
					else if (buffer[i] == 'x')
					{
						cout << "x";
					}
					else if (buffer[i] == 'o')
					{
						cout << "o";
					}
					else if (buffer[i] == ' ')
					{
						cout << "";
					}
				}
			}
			else if (bufferSplit.size()>1 && strcmp(bufferSplit[1].c_str(), "Empieza la partida") == 0)
			{

				// Recorrer cada posicion del buffer y mostrarlo
				cout << "|1|2|3|4|5|6|7|" << endl
					 << "|";
				int contador = 0;
				for (int i = 0; i < sizeof(buffer); i++)
				{

					if (buffer[i] == ';')
					{
						cout << "|" << endl;
						contador++;
						if (contador != 6)
						{
							cout << "|";
						}
					}
					else if (buffer[i] == '-')
					{
						cout << " ";
					}
					else if (buffer[i] == ',')
					{
						cout << "|";
					}
					else if (buffer[i] == 'x')
					{
						cout << "x";
					}
					else if (buffer[i] == 'o')
					{
						cout << "o";
					}
					else if (buffer[i] == ' ')
					{
						cout << "";
					}
				}
			}else{
				cout << buffer << endl;
			}

			if (strcmp(buffer, "Demasiados clientes conectados") == 0)
				fin = 1;
			else if (strcmp(buffer, "Desconexión servidor") == 0)
				fin = 1;
			else
				mostrarOpciones();
		}
		else
		{
			if (FD_ISSET(0, &auxfds))
			{

				bzero(buffer, sizeof(buffer));
				cin.getline(buffer, sizeof(buffer));
				if (strcmp(buffer, "SALIR") == 0)
				{
					fin = 1;
				}
				send(sd, buffer, sizeof(buffer), 0);
			}
		}

	} while (fin == 0);

	close(sd);
	return 0;
}