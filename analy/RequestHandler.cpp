/////////////////////////////////////////////////////////////////////
// RequestHandler.cpp - Receiver use this to handler received ms   //
//                    - for CSE 687 Project #4                     //
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
#include "RequestHandler.h"


using sPtr = std::shared_ptr < AbstractXmlElement >;

/* server's handler for request message */
void RequestHandler::operator()(Socket & socket_)
{
	Show::attach(&std::cout);
	Show::start();
	while (true)
	{
		std::string msg = socket_.recvString(); //start()
		/*socket_.recv()*/
		Show::write("\n  server recvd message \n\"" + msg + "\"");
		if (msg == "quit" || msg.size() == 0)                      // the end msg
			break;

		//parser message
		size_t pos = 0;
		HttpMessage httpmsg;
		if (msg.size() > 0)
		{
			httpmsg = httpmsg.parserHeader(msg);
		}
		
		// dispatch the command and deal with this command
		if (httpmsg.command() == "Upload")  //upload the file
		{
			UploadHanlder(httpmsg, socket_);
		}

		else if (httpmsg.command() == "DownLoad" || httpmsg.command() == "GetList") {
			DownLoadHandler(httpmsg, socket_);
		}
	
		else if (httpmsg.command() == "CONNECT" || httpmsg.command() == "Delete" || httpmsg.command() == "ShutDown")  // connect to server
		{
			handlerRecvQ.enQ(httpmsg); // enQ the swapped msg
		}

		//client get the msg
		else if (httpmsg.command() == "UPLOAD_ACK" || httpmsg.command() == "SERVER_ACK")
		{
			handlerRecvQ.enQ(httpmsg);
		}
		else if (httpmsg.command() == "DOWNLOAD_ACK") {
			DownLoadACKHandler(httpmsg, socket_);
		}
	}
}
/* save xml files in disk */
bool RequestHandler::saveXml(std::string metaData, std::string metaPath)
{
	std::ofstream file;
	file.open(metaPath);
	if (file.is_open())
		file << metaData;
	else
		return false;
	return true;
}
/* build baisc xlm meta data */
XmlDocument RequestHandler::buildBasicMetaData()
{
	sPtr pRoot = makeTaggedElement("root");
	XmlDocument doc(XmlProcessing::makeDocElement(pRoot));

	sPtr Author = makeTaggedElement("author");
	Author->addChild(makeTextElement("Wenbin Li"));
	pRoot->addChild(Author);

	sPtr Close = makeTaggedElement("close");
	Close->addChild(makeTextElement("false"));
	pRoot->addChild(Close);

	return doc;
}
/* get line in body string for dependency */
std::vector<std::string> RequestHandler::getLine(std::string bodyStr)
{
	std::vector<std::string> files;
	size_t _pos = 0;
	size_t len = 0;
	while (_pos < bodyStr.size())
	{

		std::string line = getStrLineInBody(_pos, bodyStr, bodyStr.size());
		if (line.size() == 0)
			continue;
		files.push_back(line);
	}
	return files;
}
/* get every line in string */
std::string RequestHandler::getStrLineInBody(size_t & pos, std::string str, size_t len)
{
	std::string line;
	char ch;
	while ((ch = str[pos++]) != '\n' && pos < len + 1)
	{
		line += ch;
	}
	return line;
}

/* find the value in xml, the value in xml should be trimed with \n */
bool findDependencyInXML(std::vector<std::string> depenList, std::string path) {
	
	for (auto item : depenList) {
		std::string line;
		char ch;
		size_t pos = 0;
		while ((ch = item[pos++]) != '\n' && (pos < item.size()))
		{
			line += ch;
		}

		if (line == path)
			return true;
	}
	return false;
}

/* build basic mate data and update metadata dependency */
void RequestHandler::buildMetaData(std::string MetaFilePath, HttpMessage& httpmsg)
{
	if (!FileSystem::File::exists(MetaFilePath)) {   // not exist build basic xml
		FileSystem::File WriteFile(MetaFilePath);
		WriteFile.open(FileSystem::File::out, FileSystem::File::binary); //create xml file
		WriteFile.close();
		XmlDocument doc = buildBasicMetaData();
		saveXml(doc.toString(), MetaFilePath);
	}
	if (httpmsg.body().size() > 0)
	{
		XmlParser parser(MetaFilePath);
		XmlDocument* pDoc = parser.buildDocument();
		auto depdency = getLine(httpmsg.bodyString());
		//auto existDependency = pDoc->d
		auto existDependency = XmlDocument::getElementDescendents(*pDoc, "child");
		for (auto file : depdency) {
			if (!findDependencyInXML(existDependency, file))
			{
				sPtr dep = makeTaggedElement("deps");
				dep->addChild(makeTextElement(file));
				//pDoc->element("deps").docElement()->addChild(dep);
				pDoc->xmlRoot()->addChild(dep);
			}
		}
		saveXml(pDoc->toString(), MetaFilePath);
	}
}

/* check meta data status */
bool RequestHandler::closeStatus(std::string MetaFilePath)
{
	if (!FileSystem::File::exists(MetaFilePath))    // not exist build basic xml
		return false;
	XmlParser parser(MetaFilePath);
	XmlDocument* pDoc = parser.buildDocument();
	std::vector<std::string> vectorClose = pDoc->getElementDescendents(*pDoc, "close");

	std::string line;
	char ch;
	size_t pos = 0;
	while ((ch = vectorClose[0][pos++]) != '\n' && (pos < vectorClose[0].size()))
	{
		line += ch;
	}

	if (line == "false")
		return false;
	else 
		return true;
}

/* handle upload command */
void RequestHandler::UploadHanlder(HttpMessage& httpmsg, Socket & socket_)
{
	std::string WriteFilePath;
	auto fileSize = httpmsg.fileSize();
	if (fileSize > 0) {
		auto FileName = httpmsg.findValue("FileName");
		auto dir = FileSystem::Path::getBef(FileName);     //name of package
		if (!FileSystem::Directory::exists("./Repository/" + dir)) {
			FileSystem::Directory::create("./Repository/" + dir);
		}
		std::vector<std::string> childDirs = FileSystem::Directory::getDirectories("./Repository/" + dir);
		int versionNum = childDirs.size() - 2;
		if (versionNum < 1) { // version 1, has to be created without checking close status
			versionNum++;
			FileSystem::Directory::create("./Repository/" + dir + "/" + Utilities::Converter<int>::toString(versionNum));
			WriteFilePath = "./Repository/" + dir + "/" + Utilities::Converter<int>::toString(versionNum);
			std::string xmlPath = WriteFilePath + "/" + dir + ".xml";
			buildMetaData(xmlPath, httpmsg);
		}
		else {
			std::string latestVersionPackagePath = "./Repository/" + dir + "/" + Utilities::Converter<int>::toString(versionNum);
			std::string xmlPath = latestVersionPackagePath + "/" + dir + ".xml";
			if (!closeStatus(xmlPath)) // if not close
				buildMetaData(xmlPath, httpmsg);
			else { // if package is closed, create new version
				versionNum++;
				FileSystem::Directory::create("./Repository/" + dir + "/" + Utilities::Converter<int>::toString(versionNum));
				xmlPath = "./Repository/" + dir + "/" + Utilities::Converter<int>::toString(versionNum) + "/" + dir + ".xml";
				buildMetaData(xmlPath, httpmsg);
			}
			WriteFilePath = "./Repository/" + dir + "/" + Utilities::Converter<int>::toString(versionNum);
		}
		WriteFilePath = WriteFilePath + "/" + FileName;

		writeFile(WriteFilePath, socket_, fileSize);

		httpmsg.swapURL();           // swap the url of this httpmsg
		httpmsg.setCommand("UPLOAD_ACK"); // prepare the ack for going to send to client
										  //this queue is referenced 
		handlerRecvQ.enQ(httpmsg);  // enQ the swapped msg
	}
}

/* write file in disk */
void RequestHandler::writeFile(std::string WriteFilePath, Socket & socket_, size_t fileSize)
{
	FileSystem::File WriteFile(WriteFilePath);
	WriteFile.open(FileSystem::File::out, FileSystem::File::binary);
	if (!WriteFile.isGood()) {
		std::cout << "Error while opening output file" << std::endl;
		return;
	}
	while (true) {//read buffer. The second part for the whole socket stream
		if (fileSize > 1024) {
			byte* pBuffer = new byte[1024];
			socket_.recv(1024, pBuffer);        // socket recv the buffer
			WriteFile.putBuffer(1024, pBuffer); //write file in server repository
			fileSize -= 1024;
		}
		else {
			byte* pBuffer = new byte[fileSize];
			socket_.recv(fileSize, pBuffer);        // socket recv the buffer
			WriteFile.putBuffer(fileSize, pBuffer); //write file in server repository
			WriteFile.close();                      // close the file and bread the while loop
			break;
		}
	}

}
/* receiver handle down load command */
void RequestHandler::DownLoadHandler(HttpMessage & httpmsg, Socket & socket_)
{
	httpmsg.swapURL();
	handlerRecvQ.enQ(httpmsg); // enQ the swapped msg
}
/* handle download ACK command */
void RequestHandler::DownLoadACKHandler(HttpMessage & httpmsg, Socket & socket_)
{
	auto fileSize = httpmsg.fileSize();
	if (fileSize > 0)
	{
		auto FileName = httpmsg.findValue("FileName");
		if (!FileSystem::Directory::exists("./DownLoad/"))
			FileSystem::Directory::create("./DownLoad"); //create directory

		std::string WriteFilePath = "./DownLoad/" + FileName;
		if (FileSystem::File::exists(WriteFilePath)) {         // check if the file is exist in repository
			FileSystem::File::copy(WriteFilePath, WriteFilePath + ".old");
			FileSystem::File::remove(WriteFilePath);
		}
		FileSystem::File WriteFile(WriteFilePath);
		WriteFile.open(FileSystem::File::out, FileSystem::File::binary);
		if (!WriteFile.isGood())
		{
			std::cout << "Error while opening output file" << std::endl;
			return;
		}
		while (true) //read buffer. The second part for the whole socket stream
		{
			if (fileSize > 1024)
			{
				byte* pBuffer = new byte[1024];
				socket_.recv(1024, pBuffer);        // socket recv the buffer
				WriteFile.putBuffer(1024, pBuffer); //write file in server repository
				fileSize -= 1024;
			}
			else
			{
				byte* pBuffer = new byte[fileSize];
				socket_.recv(fileSize, pBuffer);        // socket recv the buffer
				WriteFile.putBuffer(fileSize, pBuffer); //write file in server repository
				WriteFile.close();                      // close the file and bread the while loop
				break;
			}
		}
		int file_size = FileSystem::Path::get_file_size("./DownLoad/" + httpmsg.fileName());
		std::string retBody;
		if (file_size == int(httpmsg.fileSize()))
			retBody = "DownLoad SUCCESSFUL ACK for File " + httpmsg.fileName();
		else
			retBody = "DownLoad FAILED ACK for File " + httpmsg.fileName();
		httpmsg.addBody(retBody);
		handlerRecvQ.enQ(httpmsg);  // enQ the swapped msg
	}
}


#ifdef TEST_REQUESTHANDLER

int main()
{
	return 0;
}

#endif