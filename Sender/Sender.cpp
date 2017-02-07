/////////////////////////////////////////////////////////////////////
// Sender.cpp - Build connector and send messages                  //
//            - for CSE 687 Project #4                             //
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

#include "Sender.h"
/* connect to the port of 8080 */
SocketConnecter* Sender::SenderConnecter()
{
	SocketSystem ss;
	SocketConnecter* si = new SocketConnecter;
	while (!si->connect("localhost", 8080)) // client waiting to connect to server
	{
		//Show::write("\n client waiting to connect");
		::Sleep(100);
	}
	return si;
}
/* start the sender thread */
void Sender::startSendThread()
{
	stop_ = false;
	std::thread senderThread(&Sender::startSender, this);
	senderThread.detach();
}
/* the function will be called in thread of sender */
void Sender::startSender()
{
	while (!stop_)
	{
		auto processedMsg = sendQ_->deQ();
		processRequsetInServerQ(processedMsg);
	}
}

/* send the msg from sendQ to remote end */
void Sender::processRequsetInServerQ(HttpMessage& processedMsg)
{
	Show::attach(&std::cout);
	Show::start();
	// first parser command
	processedMsg = HttpMessage::parserCommand(processedMsg);
	/////////send ack to clients
	if (processedMsg.command() == "UPLOAD_ACK")
	{
		sendUloadACK(processedMsg);
	}

	if (processedMsg.command() == "SERVER_ACK") {
		sendGetListACK(processedMsg);
	}

	if (processedMsg.command() == "DOWNLOAD_ACK") {
		sendDownLoadACK(processedMsg);
	}

	////////client sender request
	if (processedMsg.command() == "Upload")
	{
		sendUpLoad(processedMsg);
	}
	if (processedMsg.command() == "GetList")
	{
		sendGetList(processedMsg);
	}

	if (processedMsg.command() == "CONNECT" || processedMsg.command() == "Delete" || processedMsg.command() == "ShutDown") 
	{
		sendConnect(processedMsg);
	}

	if (processedMsg.command() == "DownLoad")
	{
		sendDownLoad(processedMsg);
	}
}
/* send up load ack */
void Sender::sendUloadACK(HttpMessage& processedMsg)
{
	try
	{
		//new sockets to send response
		SocketSystem ssrp;
		SocketConnecter sirsp; // conetor : connet to server 
		while (!sirsp.connect("localhost", processedMsg.remotePort()))
		{
			Show::write("\n connecting to client");
			::Sleep(100);
		}
		auto t = processedMsg.toString();
		sirsp.sendString(t);

		//sirsp.shutDown();
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
/* send get list ack */
void Sender::sendGetListACK(HttpMessage & processedMsg)
{
	try
	{
		SocketSystem ssrp;
		SocketConnecter sirsp; // conetor : connet to server 
		while (!sirsp.connect("localhost", processedMsg.remotePort()))
		{
			Show::write("\n connecting to client");
			::Sleep(100);
		}
		auto t = processedMsg.toString();
		sirsp.sendString(t);
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
/* send download ack */
void Sender::sendDownLoadACK(HttpMessage & processedMsg) {
	try {
		std::string file_path = processedMsg.findValue("Path");
		std::string file_name = FileSystem::Path::getName(file_path);
		std::string package_name = FileSystem::Path::getBef(file_name);
		const int BufferLength = 1024;
		char buffer[BufferLength];
		size_t ReadLength;
		std::string ReadFilePath = file_path; // get file path
		if (!FileSystem::File::exists(ReadFilePath)) {
			std::cout << "Input doesn't exist" << std::endl;
			return;
		}
		FileSystem::File ReadFile(ReadFilePath);
		ReadFile.open(FileSystem::File::in, FileSystem::File::binary); // open file
		if (!ReadFile.isGood()) {
			std::cout << "Error while opening input file" << std::endl;
			return;
		}
		int fileSize = FileSystem::Path::get_file_size(ReadFilePath); // get file size
		std::string fileName = file_name; // get file name
		processedMsg.addAttribute(HttpMessage::attribute("FileName", fileName));
		processedMsg.addAttribute(HttpMessage::attribute("FileSize", Converter<size_t>::toString(fileSize)));
		SocketSystem ss;
		SocketConnecter* si = new SocketConnecter;
		while (!si->connect("localhost", processedMsg.remotePort())) // client waiting to connect to server
		{
			::Sleep(100);
		}
		si->sendString(processedMsg.headerString()); //send the sting msg to server . this is a pre sending infor
		while (fileSize > 0) //send files, actually sending buffer
		{
			ReadLength = ReadFile.getBuffer(BufferLength, buffer); // get buffer
			si->send(ReadLength, buffer);
			fileSize -= ReadLength; // decrease the length to know how long the file is sent
			if (ReadLength < 1024)
			{
				ReadFile.close();
				break;
			}
		}
	}
	catch (std::exception& exc) {
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}


/* sender handle upload message */
void Sender::sendUpLoad(HttpMessage & processedMsg) {
	try {
		const int BufferLength = 1024;
		char buffer[BufferLength];
		size_t ReadLength;
		std::string ReadFilePath = processedMsg.findValue("Path"); // get file path
		if (!FileSystem::File::exists(ReadFilePath)) {
			std::cout << "Input doesn't exist" << std::endl;
			return;
		}
		FileSystem::File ReadFile(ReadFilePath);
		ReadFile.open(FileSystem::File::in, FileSystem::File::binary); // open file
		if (!ReadFile.isGood()) {
			std::cout << "Error while opening input file" << std::endl;
			return;
		}
		int fileSize = FileSystem::Path::get_file_size(ReadFilePath); // get file size
		std::string fileName = FileSystem::Path::getName(ReadFilePath); // get file name
		processedMsg.addAttribute(HttpMessage::attribute("FileName", fileName));
		processedMsg.addAttribute(HttpMessage::attribute("FileSize", Converter<size_t>::toString(fileSize)));
		auto ToAddr = processedMsg.findValue("ToAddr");
		auto attrib = processedMsg.parserAtribute(ToAddr);
		size_t port = Converter<size_t>::toValue(attrib.second);
		SocketSystem ss;
		SocketConnecter* si = new SocketConnecter;
		while (!si->connect("localhost", port)) {// client waiting to connect to server
			::Sleep(100);
		}
		si->sendString(processedMsg.toString()); //send the sting msg to server . this is a pre sending infor
		while (fileSize > 0) { //send files, actually sending buffer
			ReadLength = ReadFile.getBuffer(BufferLength, buffer); // get buffer
			si->send(ReadLength, buffer);
			fileSize -= ReadLength; // decrease the length to know how long the file is sent
			if (ReadLength < 1024) {
				ReadFile.close();
				break;
			}
		}
		// display the sent message in client console
		Show::write("\n client sent \n-------------\n\"" + processedMsg.headerString() + "-------------\"");
		::Sleep(100);
		sendQiutMsg(si);
		si->shutDown();
	}
	catch (std::exception& exc) {
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

/* send get list message */
void Sender::sendGetList(HttpMessage & processedMsg)
{
	try
	{
		auto ToAddr = processedMsg.findValue("ToAddr");
		auto attrib = processedMsg.parserAtribute(ToAddr);
		size_t port = Converter<size_t>::toValue(attrib.second);
		//new sockets to send response
		SocketSystem ssrp;
		SocketConnecter sirsp; // conetor : connet to server 
		while (!sirsp.connect("localhost", port))
		{
			Show::write("\n connecting to client");
			::Sleep(100);
		}
		auto t = processedMsg.toString();
		if (t.size() > 0)
		{
			sirsp.sendString(t);
		}
		/*sendQiutMsg(&sirsp);
		sirsp.shutDown();*/
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
/* send connect message */
void Sender::sendConnect(HttpMessage & processedMsg)
{
	try
	{
		auto ToAddr = processedMsg.findValue("ToAddr");
		auto attrib = processedMsg.parserAtribute(ToAddr);
		std::string ip = attrib.first;
		if (ip == "127.0.0.1")
			ip = "localhost";
		size_t port = Converter<size_t>::toValue(attrib.second);
		//new sockets to send response
		SocketSystem ssrp;
		SocketConnecter sirsp; // conetor : connet to server 
		int num = 0;
		while (!sirsp.connect(ip, port) && num < 2)
		{
			Show::write("\n connecting to client");
			::Sleep(100);
			num++;
		}
		if (num >= 2)
			return;
		auto t = processedMsg.toString();
		if (t.size() > 0)
		{
			sirsp.sendString(t);
		}
		/*sendQiutMsg(&sirsp);
		sirsp.shutDown();*/
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
/* send download message */
void Sender::sendDownLoad(HttpMessage & processedMsg)
{
	try
	{
		auto ToAddr = processedMsg.findValue("ToAddr");
		auto attrib = processedMsg.parserAtribute(ToAddr);
		size_t port = Converter<size_t>::toValue(attrib.second);
		//new sockets to send response
		SocketSystem ssrp;
		SocketConnecter sirsp; // conetor : connet to server 
		while (!sirsp.connect("localhost", port))
		{
			Show::write("\n connecting to client");
			::Sleep(100);
		}
		auto t = processedMsg.toString();
		if (t.size() > 0)
		{
			sirsp.sendString(t);
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

/* send qiut message from clients */
void Sender::sendQiutMsg(SocketConnecter* si)
{
	std::string qiutMsg = "quit"; // send end msg
	si->sendString(qiutMsg);
	Show::write("\n  client sent \"" + qiutMsg + "\"");
	Show::write("\n");
	Show::write("\n  All done folks");
}
/* create the message for check in */
HttpMessage Sender::createMsgForCheckIn(std::string cmd, std::string ToAddr, std::string FromAddr, std::string FilePath)
{
	HttpMessage httpMsg;
	httpMsg.addAttribute(HttpMessage::attribute("Command", cmd));
	httpMsg.addAttribute(HttpMessage::attribute("ToAddr", ToAddr));
	httpMsg.addAttribute(HttpMessage::attribute("FromAddr", FromAddr));
	httpMsg.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	httpMsg.addAttribute(HttpMessage::attribute("Path", FilePath));
	return httpMsg;
}
/*create the message with time stamp  */
HttpMessage Sender::createMsgForCheckInWithTimeStamp(std::string cmd, std::string ToAddr, std::string FromAddr, std::string FilePath,
	std::string TimeStamp, std::string Body)
{
	std::string time = "2016";
	HttpMessage httpMsg;
	httpMsg.addAttribute(HttpMessage::attribute("Command", cmd));
	httpMsg.addAttribute(HttpMessage::attribute("ToAddr", ToAddr));
	httpMsg.addAttribute(HttpMessage::attribute("FromAddr", FromAddr));
	httpMsg.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	httpMsg.addAttribute(HttpMessage::attribute("Path", FilePath));
	httpMsg.addAttribute(HttpMessage::attribute("TimeStamp", time));
	httpMsg.addBody(Body);
	return httpMsg;
}

/* create msg for download request */
HttpMessage Sender::createDownloadMsg(std::string cmd, std::string ToAddr, std::string FromAddr, std::string FilePath)
{
	HttpMessage httpMsg;
	httpMsg.addAttribute(HttpMessage::attribute("Command", cmd));
	httpMsg.addAttribute(HttpMessage::attribute("ToAddr", ToAddr));
	httpMsg.addAttribute(HttpMessage::attribute("FromAddr", FromAddr));
	httpMsg.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	httpMsg.addAttribute(HttpMessage::attribute("Path", FilePath));
	return httpMsg;
}
/* get body string, this is an interface for GUI */
std::string Sender::bodyString(const HttpMessage & msg)
{
	return msg.bodyString();
}
/* parser the string style body to vector, mainly for dependency  */
std::vector<std::string> Sender::parserBodyList(std::string src)
{
	std::vector<std::string> files;
	size_t _pos = 0;
	HttpMessage httpmsg;
	while (true)    
	{
		std::string line = HttpMessage::getStrLine(_pos, src);
		if (line.size() == 0)
			continue;
		if (line == "$")
			break;
		files.push_back(line);
	}
	return files;
}



#ifdef TEST_SENDER

int main()
{
	Sender* sd = new Sender();
	//auto msgConnect = sd->createMsgForCheckIn("CONNECT", "localhost:8080", "localhost:8082", "path");
	//sd->enQ(msgConnect);
	sd->startSendThread();
	std::cout << "\n Start send thread";
	int num = 0;
	while (num < 20)
	{
		num++;
		std::cout << "\n running send thread";
	}
	sd->stop();
	std::cout << "\n stop send thread";
	return 0;
}

#endif