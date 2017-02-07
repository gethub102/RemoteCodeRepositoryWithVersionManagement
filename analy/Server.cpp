/////////////////////////////////////////////////////////////////////
// Server.cpp - Server for handle all the clients                  //
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



#include "../Server/Server.h"
/* search files */
DataStore& Server::fileSearch(int argc, char* argv[], DataStore& tmpPathDstore) {
	for (int i = 1; i < argc; ++i) {
		std::vector<std::string> pattern;
		if (handlePathAndPattern(i, argc, argv, tmpPathDstore, pattern, "-d", "-f", "-p") == 1)
			continue;
	}
	return tmpPathDstore;
}
/* handle the pattern and search files */
int Server::handlePathAndPattern(int& i, int argc, char* argv[],
	DataStore& tmpPathDstore, std::vector<std::string>& pattern,
	std::string commandD, std::string commandF, std::string commandP) {
	std::string command = argv[i];
	if (command == commandD)
	{
		i++;
		searchPatternCommand(pattern, argc, argv);
		getPath(argv[i], tmpPathDstore, pattern);
		return 0;
	}

	else if (argv[i] == commandF)
	{
		i++;
		tmpPathDstore.save(argv[i]);
		return 0;
	}

	else if (argv[i] == commandP)
	{
		i++;
		return 1;
	}
	else
	{
		i++;
		return 1;
	}
}


/*-- search the pattern command-- */
void Server::searchPatternCommand(std::vector<std::string>& pattern, int argc, char** argv) {
	std::string patterncommand = "-p";
	pattern.push_back("*.*");
	bool first = true;
	for (int j = 1; j < (argc - 1); j++)
	{
		try
		{
			if (argv[j] == patterncommand) {
				if (first == true)
				{
					first = false;
					pattern.pop_back();
				}
				pattern.push_back(argv[j + 1]);
			}
		}
		catch (const std::exception& e)
		{
			std::cout << "search pattern: " << e.what() << std::endl;
		}
	}
}
/* ---get the path of file--- */
DataStore Server::getPath(std::string path, DataStore& ds, std::vector<std::string> pattern) {
	FileMgr fm(path, ds); // path and datastore
	auto iter = pattern.begin();
	while (iter != pattern.end()) {
		fm.addPattern(*iter);
		iter++;
	}
	//fm.addPattern("*.partial");
	fm.search();
	return ds;
}

/* while loop in main thread to handle request from recvQ to sendQ */
void Server::startPrepareResponse(Receiver* recv, Sender* send)
{
	while (!stop_)
	{
		auto msg = recv->deQ();
		if (msg.command() == "ShutDown") {
			if (msg.localPort() == 8082 && (msg.localUrl() == "127.0.0.1" || msg.localUrl() == "localhost"))
			{
				recv->stop();
				send->stop();
				stop_ = true;// shut down server;
			}
			continue;
		}
		prePareReqonse(msg, send);
	}
}

/* handle request msg and then send ret to sendQ */
void Server::prePareReqonse(HttpMessage& msg, Sender* send)
{
	if (msg.command() == "UPLOAD_ACK")
	{
		prepareUpLoadACK(msg, send);
	}

	if (msg.command() == "GetList")
	{
		prepareGetList(msg, send);
	}

	if (msg.command() == "CONNECT")
	{
		msg.swapURL();
		send->enQ(msg);
	}
	if (msg.command() == "Delete")
	{
		prepareDelete(msg, send);
	}
	if (msg.command() == "DownLoad")
	{
		std::string msgBody = "DownLoad ACK for File " + msg.fileName();
		msg.addBody(msgBody);
		msg.setCommand("DOWNLOAD_ACK");
		send->enQ(msg);
	}
}
/* handle the upload ACK and send to sender */
void Server::prepareUpLoadACK(HttpMessage & msg, Sender * send)
{
	int file_size = FileSystem::Path::get_file_size(msg.filePath());
	if (file_size == int(msg.fileSize()))
	{
		std::string msgBody = "UPLOAD SUCESSFULLY";
		msg.addAttribute(HttpMessage::Attribute("Content_length", Converter<size_t>::toString(msgBody.size())));
		msg.addBody(msgBody);
	}
	else
	{
		std::string msgBody = "UPLOAD SUCESSFULLY";
		msg.addAttribute(HttpMessage::Attribute("Content_length", Converter<size_t>::toString(msgBody.size())));
		msg.addBody(msgBody);
	}
	send->enQ(msg);
}
/* prepare get list message */
void Server::prepareGetList(HttpMessage & msg, Sender * send)
{
	if (msg.filePath() == "root")
	{
		DataStore tmpPathDstore;
		char* argv[] = { "SearchAction", "-d", "./Repository" };
		int argc = 2;
		fileSearch(2, argv, tmpPathDstore);
		std::string msgBody = "\n";
		for (auto file : tmpPathDstore) {
			msgBody += file + "\n";
		}
		msgBody += "$\n";
		msg.addBody(msgBody);
	}
	msg.setCommand("SERVER_ACK");
	send->enQ(msg);
}
/* delete files */
void Server::prepareDelete(HttpMessage & msg, Sender * send)
{
	auto deletePath = msg.findValue("Path");
	FileSystem::File::remove(deletePath);
	msg.swapURL();
	msg.setCommand("SERVER_ACK");
	std::string bodyStr = "Delete Success";
	msg.addBody(bodyStr);
	send->enQ(msg);
}





#ifdef TEST_SERVER

int main(int argc, char* argv[])
{
	Show::attach(&std::cout);
	Show::start();
	StringHelper::Title("Testing sever class", '=');
	Server server;
	if (argc < 2)
	{
		std::cout << "\n sever should be specified a port number \n ";
		return 0;
	}
	std::string portFromArgv = argv[1];
	int port = Utilities::Converter<int>::toValue(portFromArgv);
	Receiver* rc = new Receiver(port); //listen and handle recved msg
	Sender* sd = new Sender(); //start send thread
	sd->startSendThread();
	server.startPrepareResponse(rc, sd); //handle the msg from recvQ
	                                     //handle ACK getlist and download
	                                     //enQ msg to sendQ
	return 0;
}

#endif