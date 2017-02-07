#ifndef MOCKCHANNEL_H
#define MOCKCHANNEL_H
/////////////////////////////////////////////////////////////////////////////
// MockChannel.h - Demo for CSE687 Project #4, Spring 2016                 //
// - build as DLL to show how C++\CLI client can use native code channel   //
// - MockChannel reads from sendQ and writes to recvQ                      //
//  ver 1.0                                                                //
//  Language:      Visual C++ 2015, SP1                                    //
//  Platform:      Macbook, Win10                                          //
//  Author:        Wenbin Li,      Syracuse University                     //
//                 wli102@syr.edu                                          //
//              Source from Dr. Jim                                        //
/////////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
* This packkage mainly build interface class ISendr, IRecvr
* and IMockChannel. These class will be use in window.h to 
* be used by GUI.
*
*  Required Files:
*  ---------------
*  Utilities.h, Utilities.cpp, MochChannel.h, MochChannel.cpp
*  BlockingQueue.h, BlockingQueue.cpp, Client.h, Client.cpp
*  HttpMessage.cpp, HttpMessage.h, FileSystem.h, FileSystem.cpp
*  Sender.cpp, Sender.h, Receiver.cpp, Receiver.h, XmlDocument.h
*  XmlDocument.cpp, XmlElement.cpp, XmlElement.h, XmlParser.cpp
*  XmlParser.h, XmlElementParts.h, XmlElementParts.cpp, Tokeniner.h
*  Tokeniner.cpp, itokcollection.h
*
*  Public Interface:
*  --------------
*  bodyString(const HttpMessage& msg)
*  startSendThread()
*  postMessage(const HttpMessage& msg)
*  getMessage()
*  startChanThread()
*  parserRcMsg(HttpMessage& retMsg)
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



#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif
#include "../HttpMessage/HttpMessage.h"
#include <string>



struct ISendr
{
	~ISendr() {};
	virtual void startSendThread() = 0;
	virtual void postMessage(const HttpMessage& msg) = 0;
	virtual HttpMessage createMsgForCheckIn(std::string cmd, std::string ToAddr, std::string FromAddr, std::string FilePath) = 0;
	virtual HttpMessage createMsgForCheckInWithTimeStamp(std::string cmd, std::string ToAddr, std::string FromAddr, std::string FilePath,
		std::string TimeStamp, std::string Body) = 0;
	virtual std::string bodyString(const HttpMessage& msg) = 0;
	virtual void stop() = 0;
	virtual std::vector<std::string> parserBodyList(std::string src) = 0;
};

struct IRecvr
{
	~IRecvr() {};
	virtual HttpMessage getMessage() = 0;
	virtual void startRecvThread(std::string port) = 0;
	virtual void stop() = 0;
};

struct IMockChannel
{
public:
	~IMockChannel() {};
	virtual void startChanThread() = 0;
	virtual void startClientThreadInWindow(ISendr* sd, IRecvr* rc) = 0;
	virtual void stop() = 0;
	virtual std::string parserRcMsg(HttpMessage& retMsg) = 0;
};

extern "C" {
	struct ObjectFactory
	{
		DLL_DECL ISendr* createSendr();
		DLL_DECL IRecvr* createRecvr();
		DLL_DECL IMockChannel* createMockChannel(ISendr* pISendr, IRecvr* pIRecvr);
	};
}

#endif


