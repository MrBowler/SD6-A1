#include <string>
#include "string.h"
#include <iostream>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")


//-----------------------------------------------------------------------------------------------
const int MESSAGE_SIZE = 1024;


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
void RunUDP()
{
	SOCKET sock;
	struct sockaddr_in sockAddr;
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
	std::string portString = "";
	while( portNumber <= 0 )
	{
		if( inputGiven )
			std::cout << "Please retry inputing the desired port number: ";
		else
			std::cout << "Please input the desired port number: ";

		std::cin >> portString;
		portNumber = atoi( portString.c_str() );
		inputGiven = true;
	}

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	sockAddr.sin_port = htons( (unsigned short) portNumber );

	if( bind( sock, (struct sockaddr *) &sockAddr, sizeof( sockAddr ) ) < 0 )
	{
		std::cout << "Failed to bind socket!\n";
		WSACleanup();
		system("PAUSE");
		return;
	}
}


//----------------------------------------------------------------------------------------------------------
void RunTCPServer()
{
	SOCKET parentSock;
	SOCKET childSock;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	struct hostent* hostInfo;
	bool inputGiven;
	char message[ MESSAGE_SIZE ];
	char* hostAddrString;
	int portNumber = 0;
	int clientLength;
	int optval;
	int sizeOfMessage;
	int queueMaxLength = 5;

	parentSock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( parentSock == INVALID_SOCKET )
	{
		std::cout << "Socket creation failed!\n";
		WSACleanup();
		system("PAUSE");
		return;
	}

	optval = 1;
	setsockopt( parentSock, SOL_SOCKET, SO_REUSEADDR, (const char *) &optval, sizeof( int ) );

	inputGiven = false;
	std::string portString = "";
	while( portNumber <= 0 )
	{
		if( inputGiven )
			std::cout << "Please retry inputing the desired port number: ";
		else
			std::cout << "Please input the desired port number: ";

		std::cin >> portString;
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
	while( true )
	{
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

		hostAddrString = inet_ntoa( clientAddr.sin_addr );
		if( hostAddrString == nullptr )
		{
			std::cout << "Failed to get host address string!\n";
			WSACleanup();
			system("PAUSE");
			return;
		}

		std::cout << "Server established connection with " << hostInfo->h_name << " (" << hostAddrString << ")\n";
		
		memset( message, 0, MESSAGE_SIZE );
		sizeOfMessage = recv( childSock, message, MESSAGE_SIZE, 0 );
		if( sizeOfMessage < 0 ) 
		{
			std::cout << "Failed to receive message!\n";
			WSACleanup();
			system("PAUSE");
			return;
		}

		std::cout << "Server received " << sizeOfMessage << " bytes: " << message << "\n";
		
		sizeOfMessage = send( childSock, message, strlen( message ), 0 );
		if( sizeOfMessage < 0 ) 
		{
			std::cout << "Failed to send echo!\n";
			WSACleanup();
			system("PAUSE");
			return;
		}

		closesocket( childSock );
	}
}


//----------------------------------------------------------------------------------------------------------
void RunTCPClient()
{
	SOCKET sock;
	struct sockaddr_in serverAddr;
	bool inputGiven;
	char message[ MESSAGE_SIZE ];
	char reply[ MESSAGE_SIZE ];
	std::string ipAddrString = "";
	u_short portNumber = 0;

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

		std::cin >> ipAddrString;
		inputGiven = true;
	}

	inputGiven = false;
	std::string portString = "";
	while( portNumber <= 0 )
	{
		if( inputGiven )
			std::cout << "Please retry inputing the desired port number: ";
		else
			std::cout << "Please input the desired port number: ";

		std::cin >> portString;
		portNumber = (u_short) atoi( portString.c_str() );
		inputGiven = true;
	}

	serverAddr.sin_addr.s_addr = inet_addr( ipAddrString.c_str() );
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( portNumber );

	if( connect( sock, (struct sockaddr *) &serverAddr, sizeof( serverAddr ) ) < 0 )
	{
		std::cout << "Failed to connect!\n";
		WSACleanup();
		system("PAUSE");
		return;
	}

	while( true )
	{
		std::cout << "Enter message: ";
		std::cin >> message;

		if( send( sock, message, strlen( message ), 0 ) < 0 )
		{
			std::cout << "Failed to send message!\n";
			WSACleanup();
			system("PAUSE");
			return;
		}

		memset( reply, 0, MESSAGE_SIZE );
		if( recv( sock, reply, MESSAGE_SIZE, 0 ) < 0 )
		{
			std::cout << "Failed to receive reply!\n";
			WSACleanup();
			system("PAUSE");
			return;
		}

		std::cout << reply << "\n";
	}

	closesocket( sock );
}


//----------------------------------------------------------------------------------------------------------
int main( int, char* )
{
	bool inputGiven;
	WSADATA wsaDt;

	if( WSAStartup( 0x202, &wsaDt ) != 0 )
	{
		std::cout << "Winsock failed to initialize.\n";
		WSACleanup();
		system( "PAUSE" );
		return 0;
	}

	inputGiven = false;
	std::string networkType = "";
	while( networkType != "tcp" && networkType != "udp" )
	{
		if( inputGiven )
			std::cout << "Please input either \'TCP\' or \'UDP\': ";
		else
			std::cout << "Do you want \'TCP\' or \'UDP\': ";

		std::cin >> networkType;
		networkType = GetLowercaseString( networkType );
		inputGiven = true;
	}

	if( networkType == "udp" )
		RunUDP();
	else if( networkType == "tcp" )
	{
		inputGiven = false;
		std::string clientOrServer = "";
		while( clientOrServer != "server" && clientOrServer != "client" )
		{
			if( inputGiven )
				std::cout << "Please input either \'server\' or \'clients\': ";
			else
				std::cout << "Do you want \'server\' or \'client\': ";

			std::cin >> clientOrServer;
			clientOrServer = GetLowercaseString( clientOrServer );
			inputGiven = true;
		}

		if( clientOrServer == "server" )
			RunTCPServer();
		else if( clientOrServer == "client" )
			RunTCPClient();
	}

	return 0;
}


//----------------------------------------------------------------------------------------------------------
int main2(void)
{
	WSADATA WsaDat;
	if(WSAStartup(MAKEWORD(2,2),&WsaDat)!=0)
	{
		std::cout<<"Winsock error - Winsock initialization failed\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}

	// Create our socket

	SOCKET Socket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(Socket==INVALID_SOCKET)
	{
		std::cout<<"Winsock error - Socket creation Failed!\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}

	// Resolve IP address for hostname
	struct hostent *host;
	if((host=gethostbyname("129.119.228.206"))==NULL)
	{
		std::cout<<"Failed to resolve hostname.\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}

	// Setup our socket address structure
	SOCKADDR_IN SockAddr;
	SockAddr.sin_port=htons(5000);
	SockAddr.sin_family=AF_INET;
	SockAddr.sin_addr.s_addr=*((unsigned long*)host->h_addr);

	// Attempt to connect to server
	/*if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr))!=0)
	{
		std::cout<<"Failed to establish connection with server\r\n";
		WSACleanup();
		system("PAUSE");
		return 0;
	}*/

	// If iMode!=0, non-blocking mode is enabled.
	u_long iMode=1;
	ioctlsocket(Socket,FIONBIO,&iMode);

	// Main loop
	for( int i=0; i < 1; ++i )
	{
		// Display message from server
		//char buffer[1000];
		//memset(buffer,0,999);
		//int inDataLength=recv(Socket,buffer,1000,0);
		//std::cout<<buffer;
		sendto( Socket, "JD Tomlinson", 12, 0, (struct sockaddr*) &SockAddr, sizeof( SockAddr ) );

		int nError=WSAGetLastError();
		if(nError!=WSAEWOULDBLOCK&&nError!=0)
		{
			std::cout<<"Winsock error code: "<<nError<<"\r\n";
			std::cout<<"Server disconnected!\r\n";
			// Shutdown our socket
			shutdown(Socket,SD_SEND);

			// Close our socket entirely
			closesocket(Socket);

			break;
		}
		Sleep(1000);
	}

	WSACleanup();
	system("PAUSE");
	return 0;
}