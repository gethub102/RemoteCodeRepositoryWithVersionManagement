#ifndef SERVER_H
#define SERVER_H
/////////////////////////////////////////////////////////////////////
// Server.h - Server for handle all the clients                    //
//          - for CSE 687 Project #4                               //
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
* This packkage main function is to build a server to handle all the 
* requests from clients, and send reply to clients.
*
*  Required Files:
*  ---------------
*  BlockingQueue.h, BlockingQueue.cpp, FileMgr.h, FileMgr.cpp
*  HttpMessage.cpp, HttpMessage.h, FileSystem.h, FileSystem.cpp
*  Sender.cpp, Sender.h, Receiver.cpp, Receiver.h, Sockets.h 
*  Sockets.cpp, Utilities.h, Utilities.cpp, Logger.cpp, Logger.h
*
*  Public Interface:
*  --------------
*  startPrepareResponse(Receiver* recv, Sender* send);
*  prePareReqonse(HttpMessage& msg, Sender* send);
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
#include <iostream>
#include "../BlockingQueue/BlockingQueue.h"
#include "../HttpMessage/HttpMessage.h"
#include "../Receiver/Receiver.h"
#include "../Sender/Sender.h"
#include "../Sockets/Sockets.h"
#include "../RequestHandler/RequestHandler.h"
#include "../FileMgr/FileSystem.h"
#include "../FileMgr/FileMgr.h"

class Server {
public:
	Server(std::string addr = "localhost", int port = 8080) : addr_(addr), port_(port) {};
	DataStore& fileSearch(int argc, char* argv[], DataStore& tmpPathDstore);
	int handlePathAndPattern(int& i, int argc, char* argv[],
		DataStore& tmpPathDstore, std::vector<std::string>& pattern,
		std::string commandD, std::string commandF, std::string commandP);
	void searchPatternCommand(std::vector<std::string>& pattern, int argc, char** argv);
	DataStore getPath(std::string path, DataStore& ds, std::vector<std::string> pattern);
	//prepare response
	void startPrepareResponse(Receiver* recv, Sender* send);
	void prePareReqonse(HttpMessage& msg, Sender* send);
	void prepareUpLoadACK(HttpMessage& msg, Sender* send);
	void prepareGetList(HttpMessage& msg, Sender* send);
	void prepareDelete(HttpMessage& msg, Sender* send);
private:
	int port_;
	std::string addr_;
	bool stop_ = false;
};


#endif