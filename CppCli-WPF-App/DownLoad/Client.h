#ifndef CLIENT_H
#define CLIENT_H

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