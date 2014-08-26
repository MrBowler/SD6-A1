#include <string>
#include "string.h"
#include <iostream>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#pragma warning( disable : 4127 )


//-----------------------------------------------------------------------------------------------
const int MESSAGE_MAX_SIZE = 1024;


//-----------------------------------------------------------------------------------------------
std::string GetLowercaseString( const std::string& str )
{
	std::string returnString = str;
	for( unsigned int charIndex = 0; charIndex < returnString.size(); ++charIndex )
	{
		if( returnString[ charIndex ] >= 'A' && returnString[ charIndex ] <= 'Z' )
		{
			returnString[ charIndex ] -= ( 'A' - 'a' );
		}
	}

	return returnString;
}


//----------------------------------------------------------------------------------------------------------
void RunServerUDP()
{
	SOCKET sock;
	sockaddr_in serverAddr;
	sockaddr_in clientAddr;
	hostent* hostInfo;
	char message[ MESSAGE_MAX_SIZE ];
	std::string portString = "";
	bool inputGiven;
	int portNumber = 0;
	int clientLength = sizeof( clientAddr );

	sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( sock == INVALID_SOCKET )
	{
		std::cout << "Socket creation failed!\n";
		WSACleanup();
		system("PAUSE");
		return;
	}

	inputGiven = false;
	while( portNumber <= 0 )
	{
		if( inputGiven )
			std::cout << "Please retry inputing the desired port number: ";
		else
			std::cout << "Please input the desired port number: ";

		std::getline( std::cin, portString );
		portNumber = atoi( portString.c_str() );
		inputGiven = true;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	serverAddr.sin_port = htons( (unsigned short) portNumber );

	if( bind( sock, (struct sockaddr *) &serverAddr, sizeof( serverAddr ) ) < 0 )
	{
		std::cout << "Failed to bind socket!\n";
		WSACleanup();
		system("PAUSE");
		return;
	}

	std::cout << "Server is now set up.\n";

	while( true )
	{
		memset( message, 0, MESSAGE_MAX_SIZE );
		if( recvfrom( sock, message, MESSAGE_MAX_SIZE, 0, (struct sockaddr*) &clientAddr, &clientLength ) < 0 )
		{
			std::cout << "Failed to receive message!\n";
			system("PAUSE");
			break;
		}

		hostInfo = gethostbyaddr( (const char *) &clientAddr.sin_addr.s_addr, sizeof( clientAddr.sin_addr.s_addr ), AF_INET );
		if( hostInfo == nullptr )
		{
			std::cout << "Failed to get host address!\n";
			system("PAUSE");
			break;
		}

		std::cout << "Received message from " << hostInfo->h_name << " (" << inet_ntoa( clientAddr.sin_addr ) << "): ";
		std::cout << message << "\n";

		if( sendto( sock, message, MESSAGE_MAX_SIZE, 0, (struct sockaddr*) &clientAddr, clientLength ) < 0 )
		{
			std::cout << "Failed to send message!\n";
			system("PAUSE");
			break;
		}

		std::cout << "Message echoed: " << message << "\n";
	}

	closesocket( sock );
	WSACleanup();
}


//----------------------------------------------------------------------------------------------------------
void RunClientUDP()
{
	SOCKET sock;
	sockaddr_in serverAddr;
	char message[ MESSAGE_MAX_SIZE ];
	std::string ipAddrString = "";
	std::string portString = "";
	bool inputGiven;
	int portNumber = 0;

	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( sock == INVALID_SOCKET )
	{
		std::cout << "Socket creation failed!\n";
		WSACleanup();
		system("PAUSE");
		return;
	}

	inputGiven = false;
	while( ipAddrString == "" )
	{
		if( inputGiven )
			std::cout << "Please retry inputing the desired IP address: ";
		else
			std::cout << "Please input the desired IP address: ";

		std::getline( std::cin, ipAddrString );
		inputGiven = true;
	}

	inputGiven = false;
	while( portNumber <= 0 )
	{
		if( inputGiven )
			std::cout << "Please retry inputing the desired port number: ";
		else
			std::cout << "Please input the desired port number: ";

		std::getline( std::cin, portString );
		portNumber = atoi( portString.c_str() );
		inputGiven = true;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr( ipAddrString.c_str() );
	serverAddr.sin_port = htons( (unsigned short) portNumber );

	while( true )
	{
		memset( message, 0, MESSAGE_MAX_SIZE );

		std::cout << "Enter message: ";
		std::cin.getline( message, MESSAGE_MAX_SIZE );

		if( sendto( sock, message, MESSAGE_MAX_SIZE, 0, (struct sockaddr*) &serverAddr, sizeof( serverAddr ) ) < 0 )
		{
			std::cout << "Failed to send message!\n";
			system("PAUSE");
			break;
		}

		memset( message, 0, MESSAGE_MAX_SIZE );
		if( recv( sock, message, MESSAGE_MAX_SIZE, 0 ) < 0 )
		{
			std::cout << "Failed to receive message!\n";
			system("PAUSE");
			break;
		}

		std::cout << "Server reply: " << message << "\n";
	}

	closesocket( sock );
	WSACleanup();
}


//----------------------------------------------------------------------------------------------------------
void RunServerTCP()
{
	SOCKET parentSock;
	SOCKET childSock;
	sockaddr_in serverAddr;
	sockaddr_in clientAddr;
	hostent* hostInfo;
	bool inputGiven;
	char message[ MESSAGE_MAX_SIZE ];
	std::string portString = "";
	int portNumber = 0;
	int clientLength;
	int queueMaxLength = 5;

	parentSock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( parentSock == INVALID_SOCKET )
	{
		std::cout << "Socket creation failed!\n";
		WSACleanup();
		system("PAUSE");
		return;
	}

	inputGiven = false;
	while( portNumber <= 0 )
	{
		if( inputGiven )
			std::cout << "Please retry inputing the desired port number: ";
		else
			std::cout << "Please input the desired port number: ";

		std::getline( std::cin, portString );
		portNumber = atoi( portString.c_str() );
		inputGiven = true;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	serverAddr.sin_port = htons( (unsigned short) portNumber );

	if( bind( parentSock, (struct sockaddr *) &serverAddr, sizeof( serverAddr ) ) < 0 )
	{
		std::cout << "Failed to bind socket!\n";
		WSACleanup();
		system("PAUSE");
		return;
	}

	if( listen( parentSock, queueMaxLength ) < 0 )
	{
		std::cout << "Failed to listen!\n";
		WSACleanup();
		system("PAUSE");
		return;
	}

	std::cout << "Server is now set up.\n";

	clientLength = sizeof( clientAddr );
	childSock = accept( parentSock, (struct sockaddr *) &clientAddr, &clientLength );
	if( childSock < 0 )
	{
		std::cout << "Failed to accept socket!\n";
		WSACleanup();
		system("PAUSE");
		return;
	}

	hostInfo = gethostbyaddr( (const char *) &clientAddr.sin_addr.s_addr, sizeof( clientAddr.sin_addr.s_addr ), AF_INET );
	if( hostInfo == nullptr )
	{
		std::cout << "Failed to get host address!\n";
		WSACleanup();
		system("PAUSE");
		return;
	}

	std::cout << "\nServer established connection with " << hostInfo->h_name << " (" << inet_ntoa( clientAddr.sin_addr ) << ")\n";

	while( true )
	{
		memset( message, 0, MESSAGE_MAX_SIZE );
		if( recv( childSock, message, MESSAGE_MAX_SIZE, 0 ) < 0 ) 
		{
			std::cout << "Failed to receive message!\n";
			system("PAUSE");
			break;
		}

		std::cout << "Message received: " << message << "\n";
		
		if( send( childSock, message, strlen( message ), 0 ) < 0 ) 
		{
			std::cout << "Failed to send echo!\n";
			system("PAUSE");
			break;
		}

		std::cout << "Message echoed: " << message << "\n";
	}

	closesocket( parentSock );
	closesocket( childSock );
	WSACleanup();
}


//----------------------------------------------------------------------------------------------------------
void RunClientTCP()
{
	SOCKET sock;
	sockaddr_in serverAddr;
	bool inputGiven;
	char message[ MESSAGE_MAX_SIZE ];
	std::string ipAddrString = "";
	std::string portString = "";
	int portNumber = 0;

	sock = socket( AF_INET , SOCK_STREAM , IPPROTO_TCP );
	if( sock == INVALID_SOCKET )
	{
		std::cout << "Failed to bind socket!\n";
		WSACleanup();
		system("PAUSE");
		return;
	}

	inputGiven = false;
	while( ipAddrString == "" )
	{
		if( inputGiven )
			std::cout << "Please retry inputing the desired IP address: ";
		else
			std::cout << "Please input the desired IP address: ";

		std::getline( std::cin, ipAddrString );
		inputGiven = true;
	}

	inputGiven = false;
	while( portNumber <= 0 )
	{
		if( inputGiven )
			std::cout << "Please retry inputing the desired port number: ";
		else
			std::cout << "Please input the desired port number: ";

		std::getline( std::cin, portString );
		portNumber = atoi( portString.c_str() );
		inputGiven = true;
	}

	std::cout << "\n";

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr( ipAddrString.c_str() );
	serverAddr.sin_port = htons( (unsigned short) portNumber );

	if( connect( sock, (struct sockaddr *) &serverAddr, sizeof( serverAddr ) ) < 0 )
	{
		std::cout << "Failed to connect!\n";
		WSACleanup();
		system("PAUSE");
		return;
	}

	while( true )
	{
		memset( message, 0, MESSAGE_MAX_SIZE );

		std::cout << "Enter message: ";
		std::cin.getline( message, MESSAGE_MAX_SIZE );

		if( send( sock, message, strlen( message ), 0 ) < 0 )
		{
			std::cout << "Failed to send message!\n";
			system("PAUSE");
			break;
		}

		memset( message, 0, MESSAGE_MAX_SIZE );
		if( recv( sock, message, MESSAGE_MAX_SIZE, 0 ) < 0 )
		{
			std::cout << "Failed to receive reply!\n";
			system("PAUSE");
			break;
		}

		std::cout << "Server reply: " << message << "\n";
	}

	closesocket( sock );
	WSACleanup();
}


//----------------------------------------------------------------------------------------------------------
int main( int, char* )
{
	bool inputGiven;
	std::string networkType = "";
	std::string clientOrServer = "";
	WSADATA wsaDt;

	if( WSAStartup( 0x202, &wsaDt ) != 0 )
	{
		std::cout << "Winsock failed to initialize.\n";
		WSACleanup();
		system( "PAUSE" );
		return 0;
	}

	inputGiven = false;
	while( networkType != "tcp" && networkType != "udp" )
	{
		if( inputGiven )
			std::cout << "Please input either \'TCP\' or \'UDP\': ";
		else
			std::cout << "Do you want \'TCP\' or \'UDP\': ";

		std::getline( std::cin, networkType );
		networkType = GetLowercaseString( networkType );
		inputGiven = true;
	}


	inputGiven = false;
	while( clientOrServer != "server" && clientOrServer != "client" )
	{
		if( inputGiven )
			std::cout << "Please input either \'server\' or \'clients\': ";
		else
			std::cout << "Do you want \'server\' or \'client\': ";

		std::getline( std::cin, clientOrServer );
		clientOrServer = GetLowercaseString( clientOrServer );
		inputGiven = true;
	}

	if( networkType == "udp" )
	{
		if( clientOrServer == "server" )
			RunServerUDP();
		else if( clientOrServer == "client" )
			RunClientUDP();
	}
	else if( networkType == "tcp" )
	{
		if( clientOrServer == "server" )
			RunServerTCP();
		else if( clientOrServer == "client" )
			RunClientTCP();
	}

	return 0;
}