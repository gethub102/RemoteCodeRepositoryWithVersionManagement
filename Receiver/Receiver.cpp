/////////////////////////////////////////////////////////////////////
// Receiver.cpp - Client package to build Client                   //
//              - Demo for CSE 687 Project #4                      //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      Macbook, Win10                                  //
//  Author:        Wenbin Li,      Syracuse University             //
//                 wli102@syr.edu                                  //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
* This packkage mainly function is to build socker listener and bind
* port to accepte the message which is sent by sender. It could use
* ResponseHandler to handle new received message.
*
*  Required Files:
*  ---------------
*  BlockingQueue.h, BlockingQueue.cpp, FileSystem.h, FileSystem.cpp
*  HttpMessage.cpp, HttpMessage.h, Receiver.cpp, Receiver.h, Logger.h
*  Logger.cpp, Socket.h, Socket.cpp, Utilities.h, Utilities.cpp
*
*  Public Interface:
*  --------------
*  getMessage()
*  startRecvThread(std::string port);
*
*  Build Command:
*  --------------
*  devenv StringSocketServer.sln /rebuild debug
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : 03 May 2016
*  - first released
*/
#include "Receiver.h"


/* start the listener thread */
Receiver::Receiver(int port)
{
	SocketSystem ss;
	SocketListener* sl = new SocketListener(port, Socket::IP6);
	sl_ = sl;
	sl_->start(*requestHandler);
}
/* start receiver thread */
void Receiver::startRecvThread(std::string port)
{
	auto intport = Converter<int>::toValue(port);
	SocketSystem ss;
	SocketListener* sl = new SocketListener(intport, Socket::IP6);
	sl_ = sl;
	sl_->start(*requestHandler);
}
/* stop receiver */
void Receiver::stop()
{
	sl_->stop();
}


#ifdef TEST_RECEIVER

int main() {
	Utilities::StringHelper::Title("Testing receiver class");
	//Listening this port and start socket thread to hanlde msg
	int port = 8089;
	Receiver* rc = new Receiver(port); // start the listener 
	std::cout << "\n start receiver thread!";
	int num = 0;
	while (num < 20)
	{
		std::cout << "\n running receiver thread!";
		num++;
		
	}
	rc->stop();
	std::cout << "\n receiver stopped!";
}

#endif 
