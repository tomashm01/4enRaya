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

bool usuarioLogged(string user, vector<Jugador *> clientes)
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

	vector<string> data;
	vector<Jugador *> clientes;
	vector<Game *> partidas;
	// contadores
	int recibidos;

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

			for (int i = 0; i < FD_SETSIZE; i++)
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
							if (clientes.size() < MAX_CLIENTS)
							{

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

							for (int j = 0; j < clientes.size(); j++)
							{
								bzero(buffer, sizeof(buffer));
								strcpy(buffer, "Desconexión servidor");
								send(clientes[j]->getIdSocket(), buffer, sizeof(buffer), 0);
								close(clientes[j]->getIdSocket());
								FD_CLR(clientes[j]->getIdSocket(), &readfds);
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

								// Elimino el usuario de la partida
								for (int j = 0; j < partidas.size(); j++)
								{
									if (partidas[j]->getJugador1()->getIdSocket() == i)
									{
										partidas.erase(partidas.begin() + j);
										// Envio mensaje de desconexion al otro jugador y cambio estado
										bzero(buffer, sizeof(buffer));
										strcpy(buffer, "+Ok. Tu oponente ha salido de la partida");
										send(partidas[j]->getJugador2()->getIdSocket(), buffer, sizeof(buffer), 0);
										partidas[j]->getJugador2()->setState(Estado::ESPERANDO);
										break;
									}
									else if (partidas[j]->getJugador2()->getIdSocket() == i)
									{
										partidas.erase(partidas.begin() + j);
										// Envio mensaje de desconexion al otro jugador y cambio estado
										bzero(buffer, sizeof(buffer));
										strcpy(buffer, "+Ok. Tu oponente ha salido de la partida");
										send(partidas[j]->getJugador1()->getIdSocket(), buffer, sizeof(buffer), 0);
										partidas[j]->getJugador1()->setState(Estado::ESPERANDO);
										break;
									}
								}
								close(i);
								FD_CLR(i, &readfds);
							}
							else
							{
								data = split(buffer, ' ');
								if (data[0] == "USUARIO" && data.size() == 2)
								{
									if (existsUser(data[1]))
									{
										// Comprobar que el usuario anterior no estaba en partida
										for (int j = 0; j < partidas.size(); j++)
										{
											if (partidas[j]->getJugador1()->getIdSocket() == i)
											{
												partidas.erase(partidas.begin() + j);
												// Envio mensaje de desconexion al otro jugador y cambio estado
												bzero(buffer, sizeof(buffer));
												strcpy(buffer, "+Ok. Tu oponente ha salido de la partida");
												send(partidas[j]->getJugador2()->getIdSocket(), buffer, sizeof(buffer), 0);
												partidas[j]->getJugador2()->setState(Estado::ESPERANDO);
												// Eliminar jugador 1 de clientes
												for (int k = 0; k < clientes.size(); k++)
												{
													if (clientes[k]->getIdSocket() == i)
													{
														clientes.erase(clientes.begin() + k);
														break;
													}
												}
												break;
											}
											else if (partidas[j]->getJugador2()->getIdSocket() == i)
											{
												partidas.erase(partidas.begin() + j);
												// Envio mensaje de desconexion al otro jugador y cambio estado
												bzero(buffer, sizeof(buffer));
												strcpy(buffer, "+Ok. Tu oponente ha salido de la partida");
												send(partidas[j]->getJugador1()->getIdSocket(), buffer, sizeof(buffer), 0);
												partidas[j]->getJugador1()->setState(Estado::ESPERANDO);
												// Eliminar jugador 2 de clientes
												for (int k = 0; k < clientes.size(); k++)
												{
													if (clientes[k]->getIdSocket() == i)
													{
														clientes.erase(clientes.begin() + k);
														break;
													}
												}
												break;
											}
										}
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
										}
										else
										{
											bzero(buffer, sizeof(buffer));
											strcpy(buffer, "-Err. Usuario ya conectado\n");
											send(i, buffer, sizeof(buffer), 0);
										}

										if (recibidos == 0)
										{
											printf("El socket %d, ha introducido ctrl+c\n", i);
											close(i);
											FD_CLR(i, &readfds);
											for (int j = 0; j < clientes.size(); j++)
											{
												if (clientes[j]->getIdSocket() == i)
												{
													clientes.erase(clientes.begin() + j);
													break;
												}
											}
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
								else if (data.size() == 1 && data[0] == "INICIAR-PARTIDA")
								{
									// Compruebo que el usuario está logueado
									bool logged = false;

									for (int j = 0; j < clientes.size(); j++)
									{
										if (clientes[j]->getIdSocket() == i && clientes[j]->getState() == Estado::LOGGED)
										{

											Jugador *jugador1 = clientes[j];
											jugador1->setState(Estado::ESPERANDO);

											// Busco un jugador en espera
											bool encontrado = false;
											logged = true;

											for (int k = 0; k < clientes.size(); k++)
											{
												if (clientes[k]->getState() == Estado::ESPERANDO && clientes[k]->getIdSocket() != i && partidas.size() <= MAX_GAMES)
												{
													Jugador *jugador2 = clientes[k];
													jugador2->setState(Estado::JUGANDO);
													jugador1->setState(Estado::JUGANDO);
													encontrado = true;
													// Creo partida
													partidas.push_back(new Game(jugador2, jugador1));
													// Envio mensaje de partida encontrada
													bzero(buffer, sizeof(buffer));
													strcpy(buffer, "+Ok.Empieza la partida. -,-,-,-,-,-,-; -,-,-,-,-,-,-; -,-,-,-,-,-,-; -,-,-,-,-,-,-; -,-,-,-,-,-,-;-,-,-,-,-,-,-;");
													send(jugador1->getIdSocket(), buffer, sizeof(buffer), 0);
													send(jugador2->getIdSocket(), buffer, sizeof(buffer), 0);

													// Envio turno al jugador que lleva mas rato esperando
													bzero(buffer, sizeof(buffer));
													strcpy(buffer, "+Ok.Turno de partida");
													send(jugador2->getIdSocket(), buffer, sizeof(buffer), 0);
													break;
												}
											}

											if (!encontrado)
											{
												bzero(buffer, sizeof(buffer));
												strcpy(buffer, "+Ok.Esperando jugadores");
												send(jugador1->getIdSocket(), buffer, sizeof(buffer), 0);
											}
										}
									}
									if (!logged)
									{
										bzero(buffer, sizeof(buffer));
										strcpy(buffer, "-Err. Debes estar logueado para jugar una partida");
										send(i, buffer, sizeof(buffer), 0);
										break;
									}
								}
								else if (data.size() == 2 && data[0] == "COLOCAR-FICHA")
								{

									// Compruebo que el usuario está jugando
									bool playing = false;
									for (int j = 0; j < clientes.size(); j++)
									{
										if (clientes[j]->getIdSocket() == i && clientes[j]->getState() == Estado::JUGANDO)
										{
											playing = true;
											break;
										}
									}

									if (!playing)
									{
										bzero(buffer, sizeof(buffer));
										strcpy(buffer, "-Err. Debes estar jugando una partida para colocar una ficha");
										send(i, buffer, sizeof(buffer), 0);
										break;
									}
									// Pasar string a int
									int numColumna = atoi(data[1].c_str());

									// Comprobar que el usuario ha introducido un numero entre 1 y 7
									if (numColumna < 0 || numColumna > 7)
									{
										bzero(buffer, sizeof(buffer));
										strcpy(buffer, "-Err. Numero de columna incorrecto.");
										send(i, buffer, sizeof(buffer), 0);
										break;
									}

									// Mostrar datos de la partida
									int idNextJugador = -1;

									for (int j = 0; j < partidas.size(); j++)
									{
										if (partidas[j]->getJugador1()->getIdSocket() == i || partidas[j]->getJugador2()->getIdSocket() == i)
										{
											if (partidas[j]->getJugador1()->getIdSocket() == i && partidas[j]->getTurno() == 1)
											{
												// Coloca la ficha el jugador 1

												if (partidas[j]->colocarFicha(numColumna) == -1)
												{
													bzero(buffer, sizeof(buffer));
													strcpy(buffer, "Err. Debe seleccionar otra columna que tenga alguna casilla disponible");
													send(i, buffer, sizeof(buffer), 0);
													break;
												}
												else
												{
													// Envia el mensaje al jugador 2
													idNextJugador = partidas[j]->getJugador2()->getIdSocket();
												}
											}
											else if (partidas[j]->getJugador2()->getIdSocket() == i && partidas[j]->getTurno() == 2)
											{
												// Coloca la ficha el jugador 2
												if (partidas[j]->colocarFicha(numColumna) == -1)
												{
													bzero(buffer, sizeof(buffer));
													strcpy(buffer, "Err. Debe seleccionar otra columna que tenga alguna casilla disponible");
													send(i, buffer, sizeof(buffer), 0);
													break;
												}
												else
												{
													// Envia el mensaje al jugador 1
													idNextJugador = partidas[j]->getJugador1()->getIdSocket();
												}
											}
											else
											{
												bzero(buffer, sizeof(buffer));
												strcpy(buffer, "-Err. Debe esperar su turno");
												send(i, buffer, sizeof(buffer), 0);
												break;
											}
										}

										// Comprobar si hay ganador
										if (partidas[j]->ganadorPartida())
										{
											bzero(buffer, sizeof(buffer));

											if (partidas[j]->getGanador() == 1)
											{
												string mensaje = "+Ok.Ganador: " + partidas[j]->getJugador1()->getUser() + " ha ganado la partida";
												strcpy(buffer, mensaje.c_str());
											}
											else if (partidas[j]->getGanador() == 2)
											{
												string mensaje = "+Ok.Ganador: " + partidas[j]->getJugador2()->getUser() + " ha ganado la partida";
												strcpy(buffer, mensaje.c_str());
											}

											send(partidas[j]->getJugador1()->getIdSocket(), buffer, sizeof(buffer), 0);
											send(partidas[j]->getJugador2()->getIdSocket(), buffer, sizeof(buffer), 0);

											partidas[j]->getJugador1()->setState(Estado::LOGGED);
											partidas[j]->getJugador2()->setState(Estado::LOGGED);

											// Eliminar partida
											partidas.erase(partidas.begin() + j);
											break;
										}

										// Comprobar si hay empate
										if (!partidas[j]->ganadorPartida() && !partidas[j]->hayEspacios())
										{
											bzero(buffer, sizeof(buffer));
											strcpy(buffer, "+Ok.Se ha producido un empate en la partida");
											send(partidas[j]->getJugador1()->getIdSocket(), buffer, sizeof(buffer), 0);
											send(partidas[j]->getJugador2()->getIdSocket(), buffer, sizeof(buffer), 0);

											partidas[j]->getJugador1()->setState(Estado::LOGGED);
											partidas[j]->getJugador2()->setState(Estado::LOGGED);

											// Eliminar partida
											partidas.erase(partidas.begin() + j);
											break;
										}
									}

									// Mostar tablero
									string mensajeTablero = "+Ok.Nuevo tablero. ";
									for (int j = 0; j < partidas.size(); j++)
									{
										if (partidas[j]->getJugador1()->getIdSocket() == i || partidas[j]->getJugador2()->getIdSocket() == i)
										{
											for (int k = 0; k < 6; k++)
											{
												for (int l = 0; l < 7; l++)
												{
													mensajeTablero += partidas[j]->getTablero(k, l);
													if (l != 6)
														mensajeTablero += ",";
												}
												mensajeTablero += "; ";
											}
											// Envio tablero actualizado a ambos jugadores
											bzero(buffer, sizeof(buffer));
											strcpy(buffer, mensajeTablero.c_str());
											send(partidas[j]->getJugador1()->getIdSocket(), buffer, sizeof(buffer), 0);
											send(partidas[j]->getJugador2()->getIdSocket(), buffer, sizeof(buffer), 0);
											if (idNextJugador != -1)
											{
												bzero(buffer, sizeof(buffer));
												strcpy(buffer, "+Ok.Turno de partida");
												send(idNextJugador, buffer, sizeof(buffer), 0);
											}
											break;
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
							close(i);
							FD_CLR(i, &readfds);

							// Encontrar posicion de la partida en el vector
							for (int j = 0; j < partidas.size(); j++)
							{
								if (partidas[j]->getJugador1()->getIdSocket() == i)
								{
									for (int k = 0; k < clientes.size(); k++)
									{
										if (clientes[k]->getIdSocket() == i)
										{
											clientes.erase(clientes.begin() + k);
											break;
										}
									}
									partidas[j]->getJugador2()->setState(Estado::LOGGED);
									partidas.erase(partidas.begin() + j);
									break;
								}
								else if (partidas[j]->getJugador2()->getIdSocket() == i)
								{
									partidas[j]->getJugador1()->setState(Estado::LOGGED);
									for (int k = 0; k < clientes.size(); k++)
									{
										if (clientes[k]->getIdSocket() == i)
										{
											clientes.erase(clientes.begin() + k);
											break;
										}
									}
									partidas.erase(partidas.begin() + j);
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	close(sd);
	return 0;
}