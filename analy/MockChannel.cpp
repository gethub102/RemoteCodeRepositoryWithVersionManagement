/////////////////////////////////////////////////////////////////////////////
// MockChannel.cpp - Demo for CSE687 Project #4, Spring 2016               //
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
#define IN_DLL
#include "MockChannel.h"
#include "../Sender/Sender.h"
#include "../Receiver/Receiver.h"
#include "../Client/Client.h"
#include <string>
#include <thread>
#include <iostream>



#ifdef TEST_MOCKCHANNEL

//----< test stub >----------------------------------------------------------

int main()
{
  ObjectFactory objFact;
  ISendr* pSendr = objFact.createSendr();
  IRecvr* pRecvr = objFact.createRecvr();
  IMockChannel* pMockChannel = objFact.createMockChannel(pSendr, pRecvr);
  pMockChannel->startChanThread();
  auto msg = pSendr->createMsgForCheckIn("GetList", "localhost:8080", "localhost:8081", "root");
  pSendr->postMessage(msg);
  pSendr->postMessage(msg);
  auto msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg.toString() << "\"";
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg.toString() << "\"";
  std::cin.get();
}
#endif

DLL_DECL ISendr * ObjectFactory::createSendr()
{
	return new Sender;
}

DLL_DECL IRecvr * ObjectFactory::createRecvr()
{
	return new Receiver;
}

DLL_DECL IMockChannel * ObjectFactory::createMockChannel(ISendr * pISendr, IRecvr * pIRecvr)
{
	return new Client(pISendr, pIRecvr);
}
