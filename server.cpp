#include "models/Game.h"
#include "models/Jugador.h"
#include <signal.h>
using namespace std;

#define MAX_CLIENTS 30
#define PORT 2060
#define MAX_GAMES 10
#define FILE "users.txt"
#define MSG_SIZE 250

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
		myfile << '\n'
			   << username + "," + password;
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

void manejador(int signum)
{
	printf("\nSe ha recibido la señal sigint\n");
	signal(SIGINT, manejador);
	exit(0);
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

void salirCliente(int socket, fd_set *readfds, int *numClientes, int arrayClientes[])
{

	char buffer[250];
	int j;

	close(socket);
	FD_CLR(socket, readfds);

	// Re-estructurar el array de clientes
	for (j = 0; j < (*numClientes) - 1; j++)
		if (arrayClientes[j] == socket)
			break;
	for (; j < (*numClientes) - 1; j++)
		(arrayClientes[j] = arrayClientes[j + 1]);

	(*numClientes)--;

	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "Desconexión del cliente: %d\n", socket);

	for (j = 0; j < (*numClientes); j++)
		if (arrayClientes[j] != socket)
			send(arrayClientes[j], buffer, sizeof(buffer), 0);
}

bool usuarioLogged(string user, vector<Jugador*> clientes)
{
	for (int i = 0; i < clientes.size(); i++)
	{
		if (strcmp(clientes[i]->getUser().c_str(), user.c_str()) == 0)
		{
			cout << "Usuario ya logueado" << endl;
			return true;
		}
	}
	return false;
}

int main()
{
	int sd, new_sd;
	struct sockaddr_in sockname, from;
	char buffer[MSG_SIZE];
	socklen_t from_len;
	fd_set readfds, auxfds;
	int salida;
	int arrayClientes[MAX_CLIENTS];
	int numClientes = 0;

	vector<string> data;
	vector<Jugador*> clientes;
	vector<Game*> partidas;
	// contadores
	int i, j, k;
	int recibidos;
	char identificador[MSG_SIZE];

	int on, ret;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
		exit(1);
	}

	// Activaremos una propiedad del socket para permitir· que otros
	// sockets puedan reutilizar cualquier puerto al que nos enlacemos.
	// Esto permite· en protocolos como el TCP, poder ejecutar un
	// mismo programa varias veces seguidas y enlazarlo siempre al
	// mismo puerto. De lo contrario habrÌa que esperar a que el puerto
	// quedase disponible (TIME_WAIT en el caso de TCP)
	on = 1;
	ret = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(PORT);
	sockname.sin_addr.s_addr = INADDR_ANY;

	if (bind(sd, (struct sockaddr *)&sockname, sizeof(sockname)) == -1)
	{
		perror("Error en la operación bind");
		exit(1);
	}
	from_len = sizeof(from);

	if (listen(sd, 1) == -1)
	{
		perror("Error en la operación de listen");
		exit(1);
	}

	// Inicializar los conjuntos fd_set
	FD_ZERO(&readfds);
	FD_ZERO(&auxfds);
	FD_SET(sd, &readfds);
	FD_SET(0, &readfds);
	signal(SIGINT, manejador);

	while (1)
	{
		auxfds = readfds;
		salida = select(FD_SETSIZE, &auxfds, NULL, NULL, NULL);

		if (salida > 0)
		{

			for (i = 0; i < FD_SETSIZE; i++)
			{

				// Buscamos el socket por el que se ha establecido la comunicación
				if (FD_ISSET(i, &auxfds))
				{

					if (i == sd) // Acepto nuevo socket
					{

						if ((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1)
						{
							perror("Error aceptando peticiones");
						}
						else
						{
							if (numClientes < MAX_CLIENTS)
							{
								arrayClientes[numClientes] = new_sd;
								numClientes++;
								FD_SET(new_sd, &readfds);
								bzero(buffer, sizeof(buffer));
								strcpy(buffer, "+OK. Usuario conectado\n");
								send(new_sd, buffer, sizeof(buffer), 0);
								// Añado el cliente al vector de clientes
								clientes.push_back(new Jugador(new_sd, Estado::NOLOGGED));
								cout << "Nuevo cliente conectado: " << new_sd << endl;
							}
							else
							{
								bzero(buffer, sizeof(buffer));
								strcpy(buffer, "Demasiados clientes conectados");
								send(new_sd, buffer, sizeof(buffer), 0);
								close(new_sd);
							}
						}
					}
					else if (i == 0) // Desconexion server
					{
						bzero(buffer, sizeof(buffer));
						cin >> buffer;
						if (strcmp(buffer, "SALIR") == 0)
						{

							for (j = 0; j < numClientes; j++)
							{
								bzero(buffer, sizeof(buffer));
								strcpy(buffer, "Desconexión servidor");
								send(arrayClientes[j], buffer, sizeof(buffer), 0);
								close(arrayClientes[j]);
								FD_CLR(arrayClientes[j], &readfds);
							}
							close(sd);
							exit(-1);
						}
					}
					else // Mensaje enviado por el cliente
					{
						bzero(buffer, sizeof(buffer));
						recibidos = recv(i, buffer, sizeof(buffer), 0);
						if (recibidos > 0)
						{
							if (strcmp(buffer, "SALIR") == 0)
							{
								// Elimino el cliente del vector de clientes
								for (int j = 0; j < clientes.size(); j++)
								{
									if (clientes[j]->getIdSocket() == i)
									{
										clientes.erase(clientes.begin() + j);
										break;
									}
								}

								//Elimino el usuario de la partida 
								for (int j = 0; j < partidas.size(); j++)
								{
									if (partidas[j]->getJugador1()->getIdSocket() == i)
									{
										partidas.erase(partidas.begin() + j);
										//Envio mensaje de desconexion al otro jugador y cambio estado
										bzero(buffer, sizeof(buffer));
										strcpy(buffer, "+Ok. Tu oponente ha salido de la partida");
										send(partidas[j]->getJugador2()->getIdSocket(), buffer, sizeof(buffer), 0);
										partidas[j]->getJugador2()->setState(Estado::ESPERANDO);
										break;
									}
									else if (partidas[j]->getJugador2()->getIdSocket() == i)
									{
										partidas.erase(partidas.begin() + j);
										//Envio mensaje de desconexion al otro jugador y cambio estado
										bzero(buffer, sizeof(buffer));
										strcpy(buffer, "+Ok. Tu oponente ha salido de la partida");
										send(partidas[j]->getJugador1()->getIdSocket(), buffer, sizeof(buffer), 0);
										partidas[j]->getJugador1()->setState(Estado::ESPERANDO);
										break;
									}
								}
								salirCliente(i, &readfds, &numClientes, arrayClientes);

							}
							else
							{
								data = split(buffer, ' ');
								if (data[0] == "USUARIO" && data.size() == 2)
								{
									if (existsUser(data[1]))
									{
										if (!usuarioLogged(data[1], clientes))
										{
											// Comprobar existe usuario
											bzero(buffer, sizeof(buffer));
											strcpy(buffer, "+OK.Usuario correcto\n");
											send(i, buffer, sizeof(buffer), 0);

											// Cambiar estado del cliente
											for (int j = 0; j < clientes.size(); j++)
											{
												if (clientes[j]->getIdSocket() == i)
												{
													clientes[j]->setState(Estado::LOGGEDWITHOUTPASSWORD);
													clientes[j]->setUser(data[1]);
												}
											}
										}else{
											bzero(buffer, sizeof(buffer));
											strcpy(buffer, "-Err. Usuario ya conectado\n");
											send(i, buffer, sizeof(buffer), 0);
										}

										if (recibidos == 0)
										{
											printf("El socket %d, ha introducido ctrl+c\n", i);
											salirCliente(i, &readfds, &numClientes, arrayClientes);
										}
									}
									else
									{
										bzero(buffer, sizeof(buffer));
										strcpy(buffer, "-Err. Usuario incorrecto\n");
										send(i, buffer, sizeof(buffer), 0);
									}
								}
								else if (data.size() == 2 && data[0] == "PASSWORD")
								{
									// Obtener usuario del cliente
									string user = "";
									for (int j = 0; j < clientes.size(); j++)
									{
										if (clientes[j]->getIdSocket() == i)
										{
											user = clientes[j]->getUser();
										}
									}
									if (loginUser(user, data[1]))
									{
										bzero(buffer, sizeof(buffer));
										strcpy(buffer, "+OK. Usuario validado\n");
										send(i, buffer, sizeof(buffer), 0);

										// Modifico estado del cliente
										for (int j = 0; j < clientes.size(); j++)
										{
											if (clientes[j]->getIdSocket() == i)
											{
												clientes[j]->setPass(data[1]);
												clientes[j]->setState(Estado::LOGGED);
											}
										}
									}
									else
									{
										bzero(buffer, sizeof(buffer));
										strcpy(buffer, "-Err. Error en la validación\n");
										send(i, buffer, sizeof(buffer), 0);
									}
								}
								else if (data[0] == "REGISTRO" && data[1] == "-u" && data[3] == "-p" && data.size() == 5)
								{ // Registro de usuario
									if (existsUser(data[2]))
									{
										bzero(buffer, sizeof(buffer));
										strcpy(buffer, "-Err. Usuario ya existe\n");
										send(i, buffer, sizeof(buffer), 0);
									}
									else
									{
										if (registerUser(data[2], data[4]))
										{
											bzero(buffer, sizeof(buffer));
											strcpy(buffer, "+OK. Usuario registrado\n");
											send(i, buffer, sizeof(buffer), 0);
											
											// Modifico estado del cliente
											for (int j = 0; j < clientes.size(); j++)
											{
												if (clientes[j]->getIdSocket() == i)
												{
													clientes[j]->setUser(data[2]);
													clientes[j]->setPass(data[4]);
													clientes[j]->setState(Estado::LOGGED);
												}
											}
										}
										else
										{
											bzero(buffer, sizeof(buffer));
											strcpy(buffer, "-Err. Error al registrar usuario\n");
											send(i, buffer, sizeof(buffer), 0);
										}
									}
								}
								else if(data.size() == 1 && data[0] == "INICIAR-PARTIDA"){
									//Compruebo que el usuario está logueado
									
									for (int j = 0; j < clientes.size(); j++)
									{
										if (clientes[j]->getIdSocket() == i)
										{
											if(clientes[j]->getState() == Estado::LOGGED){
												Jugador* jugador1=clientes[j];
												jugador1->setState(Estado::ESPERANDO);

												//Busco un jugador en espera
												bool encontrado = false;
												
												for (int k=0; k < clientes.size(); k++)
												{
													if (clientes[k]->getState() == Estado::ESPERANDO && clientes[k]->getIdSocket() != i)
													{
														Jugador* jugador2=clientes[k];
														jugador2->setState(Estado::JUGANDO);
														jugador1->setState(Estado::JUGANDO);
														encontrado = true;
														//Creo partida
														partidas.push_back(new Game(jugador1, jugador2));
														//Envio mensaje de partida encontrada
														bzero(buffer, sizeof(buffer));
														strcpy(buffer, "+Ok.Empieza la partida");
														send(jugador1->getIdSocket(), buffer, sizeof(buffer), 0);
														send(jugador2->getIdSocket(), buffer, sizeof(buffer), 0);
														break;
													}
													
												}

												if(!encontrado){
													bzero(buffer, sizeof(buffer));
													strcpy(buffer, "+Ok.Esperando jugadores");
													send(jugador1->getIdSocket(), buffer, sizeof(buffer), 0);
												}

											}
										}
									}
								}
								else if(data.size() == 2 && data[0] == "COLOCAR-FICHA"){
									//Comprobar que el jugador esta jugando
									for(int j=0<j<clientes.size();j++;){
										if(clientes[j]->getIdSocket()==i && clientes[j]->getState()==Estado::JUGANDO){
											//Comprobar que la partida existe
											
										}
									}
								}
								else
								{
									// Ninguna opcion anterior seleccionada
									bzero(buffer, sizeof(buffer));
									strcpy(buffer, "No has seleccionado ninguna de las opciones correctas\n");
									send(i, buffer, sizeof(buffer), 0);
								}
							}
						}
						if (recibidos == 0)
						{
							printf("El socket %d, ha introducido ctrl+c\n", i);
							salirCliente(i, &readfds, &numClientes, arrayClientes);
						}
					}
				}
			}
		}
	}
	close(sd);
	return 0;
}