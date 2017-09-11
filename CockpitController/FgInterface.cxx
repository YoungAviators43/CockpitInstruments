/// File: FgInterface.cxx

/// UDP interface to flightgear aircraft simulator.

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
using namespace std;

void errExit(const char *msg);
void ioControlAddFgRead(int fd);
void ioControlAddFgWrite(int fd);
void ioControlRemoveFgWrite(int fd);

static int socketIn;
static int socketOut;
static struct sockaddr_in server_addr_out;
        
void initFgInterfaceIn(int portIn)
{
  struct sockaddr_in server_addr_in;

  // host config        
  socketIn = socket(AF_INET, SOCK_DGRAM, 0);
  if (socketIn == -1)
  {
    errExit("SocketInInit");
  }

  bzero(&server_addr_in, sizeof(server_addr_in));
  server_addr_in.sin_family = AF_INET;
  server_addr_in.sin_port = htons(portIn);
  server_addr_in.sin_addr.s_addr = INADDR_ANY;

  if (bind(socketIn,(struct sockaddr *)&server_addr_in,sizeof(struct sockaddr)) == -1)
  {
    errExit("SocketInBind");
  }

  cout << "UDPServer Waiting for client on port " << portIn << endl;
  ioControlAddFgRead(socketIn);
}

void initFgInterfaceOut(int portOut, char *ipname)
{
  // client config
  if ((socketOut = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
  {
    errExit("SocketOutInit");
  }

  bzero(&server_addr_out, sizeof(server_addr_out));
  server_addr_out.sin_family = AF_INET;
  server_addr_out.sin_port = htons(portOut);
  if (inet_aton(ipname, &server_addr_out.sin_addr) == 0)
  {
    errExit("inet_aton");
  }
}

void receiveFgPacket(char *buffer, size_t bufferSize)
{
  int bytesRead;
  struct sockaddr_in client_addr_in;
  socklen_t addr_len;

  // wait for new UDP packet, and read it in to recv_data
  addr_len = sizeof(struct sockaddr);
  bytesRead = recvfrom(socketIn, buffer, bufferSize, 0, (struct sockaddr *)&client_addr_in, &addr_len);
  buffer[bytesRead] = '\0';

#if 0
  // print where it got the UDP data from and the raw data
  cout << "(" << inet_ntoa(client_addr_in.sin_addr) << ", " << ntohs(client_addr_in.sin_port) << "): "
       << buffer << endl;
#endif

  ioControlAddFgWrite(socketOut);
}

//void displayAddFgWrite(uint32_t count);

void sendFgPacket(char *buffer)
{
  //  static uint32_t packetCount = 0;

  // Send UDP packet
  if (sendto(socketOut,
	     buffer, strlen(buffer), 0,
	     (struct sockaddr *)&server_addr_out, sizeof(struct sockaddr)) == -1)
  {
    // report packet send error
  }
  else
  {
    //    displayAddFgWrite(packetCount++);
  }
  ioControlRemoveFgWrite(socketOut);
}
