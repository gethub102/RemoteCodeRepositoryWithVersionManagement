#ifndef CLIENT_H
#define CLIENT_H
/////////////////////////////////////////////////////////////////////
// Client.h - Client package to build Client                       //
//          - Demo for CSE 687 Project #4                          //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      Macbook, Win10                                  //
//  Author:        Wenbin Li,      Syracuse University             //
//                 wli102@syr.edu                                  //
//              Source from Dr. Jim                                //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
* This packkage mainly use sender and receiver to build channle. This channel
* is to communication to server. Client could use receiver to build listener
* to get response and build sender to connect to server, then send msg to it.
*
*  Required Files:
*  ---------------
*  Window.h, Window.cpp, MochChannel.h, MochChannel.cpp,
*  BlockingQueue.h, BlockingQueue.cpp, Client.h, Client.cpp
*  HttpMessage.cpp, HttpMessage.h, FileSystem.h, FileSystem.cpp
*  Sender.cpp, Sender.h, Receiver.cpp, Receiver.h, XmlDocument.h
*  XmlDocument.cpp, XmlElement.cpp, XmlElement.h, XmlParser.cpp
*  XmlParser.h, XmlElementParts.h, XmlElementParts.cpp, Tokeniner.h
*  Tokeniner.cpp, itokcollection.h
*
*  Public Interface:
*  --------------
*  startClientThread()
*  startChanThread()
*  stop()
*  startChannel()
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

#include "../Sockets/Sockets.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../HttpMessage/HttpMessage.h"
#include "../FileMgr/FileSystem.h"
#include "../BlockingQueue/BlockingQueue.h"
#include "../Receiver/Receiver.h"
#include "../Sender/Sender.h"
#include <string>
#include <iostream>
#include <thread>
#include "../MockChannel/MockChannel.h"

class Client  : public IMockChannel
{
public:
	Client(int port) { port_ = port; };
	Client(ISendr* sd, IRecvr* rc) :sd_(sd), rc_(rc)  {};
	void startClientThread(Sender* sd, Receiver* rc);
	void startClientThreadInWindow(ISendr* sd, IRecvr* rc);

	void startChanThread();
	void stop();
	void startChannel();

	std::string parserRcMsg(HttpMessage& retMsg);
private:
	int port_;
	IRecvr* rc_;
	ISendr* sd_;
	bool stop_ = false;
};


#endif