/////////////////////////////////////////////////////////////////////
// Client.cpp - Client package to build Client                     //
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
*  BlockingQueue.h, BlockingQueue.cpp, Client.h, Client.cpp
*  HttpMessage.cpp, HttpMessage.h, FileSystem.h, FileSystem.cpp
*  Sender.cpp, Sender.h, Receiver.cpp, Receiver.h, XmlDocument.h
*  Logger.cpp, Logger.h
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
*  ver 1.0 : 16 May 2016
*  - first released
*/
#include "Client.h"

using Show = StaticLogger<1>;
using namespace Utilities;
/* start the client thread */
void Client::startClientThread(Sender* sd, Receiver* rc)
{
	while (true)
	{
		auto retMsg = rc->deQ();
		parserRcMsg(retMsg);
	}
}
/* start client in window */
void Client::startClientThreadInWindow(ISendr * sd, IRecvr * rc)
{
	while (true)
	{
		auto retMsg = rc->getMessage();
		if (retMsg.command() == "UPLOAD_ACK" || retMsg.command() == "SERVER_ACK" || retMsg.command() == "DOWNLOAD_ACK")
		{
			auto ret = retMsg.bodyString();
			Show::write("\n client get information: " + ret);
		}
		else if (retMsg.command() == "UPLOAD_FAILED") { // if failed request agin
			retMsg.swapURL();
			retMsg.setCommand("Upload");
			sd->postMessage(retMsg);
		}
		else if (retMsg.command() == "DOWNLOAD_FAILED") { // if failed request agin
			retMsg.swapURL();
			retMsg.setCommand("Download");
			sd->postMessage(retMsg);
		}
	}
}
/* start channel thread */
void Client::startChanThread()
{
	std::thread  channel(&Client::startChannel, this);
	channel.detach();
}
/* stop the thread */
void Client::stop()
{
	stop_ = true;
}
/* start channel */
void Client::startChannel()
{
	Sender* sd = dynamic_cast<Sender*>(sd_);
	Receiver* rc = dynamic_cast<Receiver*>(rc_);
	if (sd == nullptr || rc == nullptr)
	{
		std::cout << "\n  failed to start Channel\n\n";
		return;
	}
	while (!stop_)
	{
		auto retMsg = rc->getMessage();
		if (retMsg.command() == "UPLOAD_ACK" || retMsg.command() == "SERVER_ACK" || retMsg.command() == "DOWNLOAD_ACK")
		{
			auto ret = retMsg.bodyString();
			Show::write("\n client get information: " + ret);
		}
		else if (retMsg.command() == "UPLOAD_FAILED") { // if failed request agin
			retMsg.swapURL();
			retMsg.setCommand("Upload");
			sd->postMessage(retMsg);
		}
		else if (retMsg.command() == "DOWNLOAD_FAILED") { // if failed request agin
			retMsg.swapURL();
			retMsg.setCommand("Download");
			sd->postMessage(retMsg);
		}
	}
}

/* interface for parser recved msg in windows receive thread getmessage */
std::string Client::parserRcMsg(HttpMessage & retMsg)
{
	if (retMsg.command() == "UPLOAD_ACK" || retMsg.command() == "SERVER_ACK" || retMsg.command() == "DOWNLOAD_ACK")
	{
		auto ret = retMsg.bodyString();
		Show::write("\n client get information: " + ret);
		return ret;
	}
	else if (retMsg.command() == "CONNECT") {
		std::string ret = "\n Connect has been done!";
		Show::write("\n client get information: " + ret);
		return ret;
	}
	else if (retMsg.command() == "UPLOAD_FAILED") { // if failed request agin
		retMsg.swapURL();
		retMsg.setCommand("Upload");
		sd_->postMessage(retMsg);
		return "upload failed";
	}
	else if (retMsg.command() == "DOWNLOAD_FAILED") { // if failed request agin
		retMsg.swapURL();
		retMsg.setCommand("Download");
		sd_->postMessage(retMsg);
		return "download failed";
	}
	return "command failed";
}


#ifdef TEST_CLIENT

int main()
{
	StringHelper::Title("Testing client class", '=');
	int port = 8081;
	Client client(port);// (port);
	Receiver* rc = new Receiver(port);  // client start listening this port
	// construct the httpmsg
	HttpMessage httpmsg;
	httpmsg.addAttribute(HttpMessage::attribute("Command", "Upload"));
	httpmsg.addAttribute(HttpMessage::attribute("ToAddr", "127.0.0.1:8080"));
	httpmsg.addAttribute(HttpMessage::attribute("FromAddr", "127.0.0.1:8081"));
	httpmsg.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	httpmsg.addAttribute(HttpMessage::attribute("Path", "./test.png"));

	HttpMessage httpmsg2;
	httpmsg2.addAttribute(HttpMessage::attribute("Command", "Upload"));
	httpmsg2.addAttribute(HttpMessage::attribute("ToAddr", "127.0.0.1:8080"));
	httpmsg2.addAttribute(HttpMessage::attribute("FromAddr", "127.0.0.1:8081"));
	httpmsg2.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	httpmsg2.addAttribute(HttpMessage::attribute("Path", "./test.txt"));

	HttpMessage httpmsg3;
	httpmsg3.addAttribute(HttpMessage::attribute("Command", "GetList"));
	httpmsg3.addAttribute(HttpMessage::attribute("ToAddr", "127.0.0.1:8080"));
	httpmsg3.addAttribute(HttpMessage::attribute("FromAddr", "127.0.0.1:8081"));
	httpmsg3.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	httpmsg3.addAttribute(HttpMessage::attribute("Path", "root"));

	HttpMessage httpmsg4;
	httpmsg4.addAttribute(HttpMessage::attribute("Command", "DownLoad"));
	httpmsg4.addAttribute(HttpMessage::attribute("ToAddr", "127.0.0.1:8080"));
	httpmsg4.addAttribute(HttpMessage::attribute("FromAddr", "127.0.0.1:8081"));
	httpmsg4.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	httpmsg4.addAttribute(HttpMessage::attribute("Path", "./Repository/test2/3/test2.png"));

	Sender* sd = new Sender();
	sd->enQ(httpmsg);
	sd->enQ(httpmsg2);
	sd->enQ(httpmsg3);
	sd->enQ(httpmsg4);
	sd->startSendThread();

	client.startClientThread(sd, rc);

}

#endif


