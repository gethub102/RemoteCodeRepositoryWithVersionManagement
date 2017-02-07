/////////////////////////////////////////////////////////////////////
// Client2.cpp - Client Implementation                             //
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
*  Build Command:
*  --------------
*  devenv StringSocketServer.sln /rebuild debug
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : 03 May 2016
*  - first released
*/
#include "../Client/Client.h"
void msgCreate(HttpMessage& httpmsg) {
	httpmsg.addAttribute(HttpMessage::attribute("Command", "Upload"));
	httpmsg.addAttribute(HttpMessage::attribute("ToAddr", "127.0.0.1:8080"));
	httpmsg.addAttribute(HttpMessage::attribute("FromAddr", "127.0.0.1:8082"));
	httpmsg.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	httpmsg.addAttribute(HttpMessage::attribute("Path", "./test2.png"));
}

void msgCreate2(HttpMessage& httpmsg2) {
	httpmsg2.addAttribute(HttpMessage::attribute("Command", "Upload"));
	httpmsg2.addAttribute(HttpMessage::attribute("ToAddr", "127.0.0.1:8080"));
	httpmsg2.addAttribute(HttpMessage::attribute("FromAddr", "127.0.0.1:8082"));
	httpmsg2.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	httpmsg2.addAttribute(HttpMessage::attribute("Path", "./test2.txt"));
}

HttpMessage createDeletMsg(std::string deletePath) {
	HttpMessage msg;
	msg.addAttribute(HttpMessage::attribute("Command", "Delete"));
	msg.addAttribute(HttpMessage::attribute("ToAddr", "127.0.0.1:8080"));
	msg.addAttribute(HttpMessage::attribute("FromAddr", "127.0.0.1:8082"));
	msg.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	msg.addAttribute(HttpMessage::attribute("Path", deletePath));
	return msg;
}

HttpMessage shutDownServerMsg() {
	HttpMessage msg;
	msg.addAttribute(HttpMessage::attribute("Command", "ShutDown"));
	msg.addAttribute(HttpMessage::attribute("ToAddr", "127.0.0.1:8080"));
	msg.addAttribute(HttpMessage::attribute("FromAddr", "127.0.0.1:8082"));
	msg.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	return msg;
}

#ifdef TEST_CLIENT2

int main(int argc, char* argv[])
{
	StringHelper::Title("Testing client2 class", '=');
	Utilities::putline();

	if (argc < 2)
	{
		std::cout << "\n client2 should be specified a port number \n ";
		return 0;
	}
	std::string portFromArgv = argv[1];
	int port = Utilities::Converter<int>::toValue(portFromArgv);

	StringHelper::Title("Demonstrate Requirements", '=');
	HttpMessage uploadmsg; msgCreate(uploadmsg);     // construct the httpmsg
	HttpMessage uploadmsg2; msgCreate2(uploadmsg2);  // construct the httpmsg
	
	Client client(port);
	Receiver* rc = new Receiver(port); // start the listener 
	Sender* sd = new Sender();
	sd->startSendThread();

	auto msgConnect = sd->createMsgForCheckIn("CONNECT", "localhost:8080", "localhost:8082", "path");
	std::string time = "2016";
	HttpMessage dependencyMsg = sd->createMsgForCheckInWithTimeStamp("Upload", "localhost:8080", "localhost:8082", argv[2], time, "");
	std::string depBody = "./Repository/test2.txt\n./Repository/test2.png";
	dependencyMsg.addBody(depBody);
	HttpMessage downloadMsg = sd->createDownloadMsg("DownLoad", "localhost:8080", "localhost:8082", "./Repository/test2/1/test2.png");
	HttpMessage deletMsg = createDeletMsg("./Repository/test2/1/test2Copy.png");
	HttpMessage shutDownMsg = shutDownServerMsg();
	//sd->enQ(msgConnect);
	//sd->enQ(uploadmsg);
	//sd->enQ(uploadmsg2);
	//sd->enQ(dependencyMsg);
	//sd->enQ(downloadMsg);
	//sd->enQ(deletMsg);
	
	int count = 10;
	while (count > 0)
	{
		std::cout << "\n shut down the server " << count << std::endl;
		count--;
		Sleep(1000);
	}
	sd->enQ(shutDownMsg);
	client.startClientThread(sd, rc);
}

#endif