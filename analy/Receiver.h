#ifndef RECEIVER_H
#define RECEIVER_H
/////////////////////////////////////////////////////////////////////
// Receiver.h - Client package to build Client                     //
//            - Demo for CSE 687 Project #4                        //
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
#include "../BlockingQueue/BlockingQueue.h"
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../RequestHandler/RequestHandler.h"
#include <map>
#include <functional>
#include "../MockChannel/MockChannel.h"

class Receiver : public IRecvr
{
public:
	~Receiver() { delete recvQ_; delete requestHandler; };
	Receiver() {};
	Receiver(int port);
	HttpMessage deQ() { return recvQ_->deQ(); };
	HttpMessage& message() { return httpMsg_; };
	HttpMessage getMessage() { return deQ(); };
	void startRecvThread(std::string port);
	void stop();
private:
	//socket listener
	SocketSystem ss;
	SocketListener* sl_ = new SocketListener(8080, Socket::IP6);

	std::string rcvPath_;
	// queue referenced by client handler
	BlockingQueue<HttpMessage>* recvQ_ = new BlockingQueue<HttpMessage>;
	// client handler reference the recv q as shared
	RequestHandler* requestHandler = new RequestHandler(*recvQ_);
	HttpMessage httpMsg_;
};

#endif
