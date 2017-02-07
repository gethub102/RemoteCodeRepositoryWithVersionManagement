#ifndef REQUESTHANLDER_H
#define REQUESTHANLDER_H
/////////////////////////////////////////////////////////////////////
// RequestHandler.h - Receiver use this to handler received ms     //
//                  - for CSE 687 Project #4                       //
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
* This package is used to handle received message, store files, parser
* msg and enQ to receive queue.
*
*  Required Files:
*  ---------------
*  BlockingQueue.h, BlockingQueue.cpp, Utilities.h, Utilities.cpp
*  HttpMessage.cpp, HttpMessage.h, FileSystem.h, FileSystem.cpp
*  XmlDocument.cpp, XmlElement.cpp, XmlElement.h, XmlParser.cpp
*  XmlParser.h, XmlElementParts.h, XmlElementParts.cpp, Tokeniner.h
*  Tokeniner.cpp, itokcollection.h, Logger.cpp, Logger.h
*
*  Public Interface:
*  --------------
*  operator()(Socket& socket_);
*  saveXml(std::string metaData, std::string metaPath)
*  buildBasicMetaData();
*  closeStatus(std::string metaDataPath);
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
#include "../HttpMessage/HttpMessage.h"
#include "../Utilities/Utilities.h"
#include "../FileMgr/FileSystem.h"
#include "../XmlDocument/XmlDocument.h"

using Show = StaticLogger<1>;
using Attribute = HttpMessage::Attribute;
using Attributes = HttpMessage::Attributes;
using Name = HttpMessage::Name;
using Value = HttpMessage::Value;
using Body = HttpMessage::Body;
using byte = HttpMessage::byte;

using namespace Utilities;
using namespace XmlProcessing;


class RequestHandler
{
public:
	RequestHandler():handlerRecvQ(*q) { };
	RequestHandler(BlockingQueue<HttpMessage>& msgQ) : handlerRecvQ(msgQ){  }; //reference to blocking Q
	void operator()(Socket& socket_);
	RequestHandler(const RequestHandler&& other) : handlerRecvQ(other.handlerRecvQ) {}
	bool saveXml(std::string metaData, std::string metaPath);
	XmlDocument buildBasicMetaData();
	std::vector<std::string> getLine(std::string bodyStr);
	std::string getStrLineInBody(size_t& pos, std::string str, size_t len);
	void buildMetaData(std::string MetaFilePath, HttpMessage& httpmsg);
	bool closeStatus(std::string metaDataPath);

	void UploadHanlder(HttpMessage& httpmsg, Socket & socket_);
	void writeFile(std::string WriteFilePath, Socket & socket_, size_t fileSize);
	void DownLoadHandler(HttpMessage& httpmsg, Socket & socket_);
	void DownLoadACKHandler(HttpMessage& httpmsg, Socket & socket_);
private:
	BlockingQueue<HttpMessage>& handlerRecvQ;
	BlockingQueue<HttpMessage>* q = new BlockingQueue<HttpMessage>;
};

#endif 