#include "models/Client.cpp"
using namespace std;

int main ( )
{
    Client client;
	client.connectToServer();
	do
	{
		client.freeBuffer();
		cout<<"Enviar mensaje: ";
		char msg[100];
		cin.getline(msg, 100);
		client.sendMsg(msg);
	}while(strcmp(client.getBuffer(), "0") != 0);
	client.closeConnection();
	return 0;
}
		
