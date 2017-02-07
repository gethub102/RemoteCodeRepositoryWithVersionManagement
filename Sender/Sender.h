#ifndef SENDER_H
#define SENDER_H
/////////////////////////////////////////////////////////////////////
// Sender.h - Build connector and send messages                    //
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
* This packkage main function is to connect to remote receiver and 
* send message to it. And this package could provide the interface 
* for the use of GUI client so that window could postMessage, create
* message.
*
*  Required Files:
*  ---------------
*  MochChannel.h, MochChannel.cpp, Sender.h Sender.cpp, Logger.h
*  BlockingQueue.h, BlockingQueue.cpp, Logger.cpp, Sockets.h
*  HttpMessage.cpp, HttpMessage.h, FileSystem.h, FileSystem.cpp
*  Sockets.cpp
*
*  Public Interface:
*  --------------
*  startSendThread();
*  postMessage(const HttpMessage& msg)
*  createMsgForCheckIn()
*  createDownloadMsg()
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
#include "../Logger/Logger.h"
#include "../FileMgr/FileSystem.h"
#include "../MockChannel/MockChannel.h"

using Show = StaticLogger<1>;

class Sender : public ISendr
{
public:
	~Sender() { delete sendQ_; };
	
	void enQ(HttpMessage msg) { sendQ_->enQ(msg); };
	SocketConnecter* SenderConnecter();

	void startSendThread();
	void startSender();
	void processRequsetInServerQ(HttpMessage& processedMsg);
	void sendUloadACK(HttpMessage& processedMsg);
	void sendGetListACK(HttpMessage& processedMsg);
	void sendDownLoadACK(HttpMessage& processedMsg);
	void sendUpLoad(HttpMessage& processedMsg);
	void sendGetList(HttpMessage& processedMsg);
	void sendConnect(HttpMessage& processedMsg);
	void sendDownLoad(HttpMessage& processedMsg);

	void sendQiutMsg(SocketConnecter* si);
	void postMessage(const HttpMessage& msg) { enQ(msg); };
	HttpMessage& message() { return httpMsg_; };
	
	HttpMessage createMsgForCheckIn(std::string cmd, std::string ToAddr, std::string FromAddr, std::string FilePath);
	HttpMessage createMsgForCheckInWithTimeStamp(std::string cmd, std::string ToAddr, std::string FromAddr, std::string FilePath, 
		std::string TimeStamp, std::string Body);
	HttpMessage createDownloadMsg(std::string cmd, std::string ToAddr, std::string FromAddr, std::string FilePath);
	std::string bodyString(const HttpMessage& msg);
	void stop() { stop_ = true; };
	std::vector<std::string> parserBodyList(std::string src);
private:
	BlockingQueue<HttpMessage>* sendQ_ = new BlockingQueue<HttpMessage>;
	HttpMessage httpMsg_;
	bool stop_ = false;
};

#endif
