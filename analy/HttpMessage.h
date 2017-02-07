#ifndef HTTPMESSAGE_H
#define HTTPMESSAGE_H
/////////////////////////////////////////////////////////////////////
// HttpMessage.h - HttpMessage for channel communicatioin          //
//               - for CSE 687 Project #4                          //
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
* This packkage can create and parser http style message to fulfill 
* usage in channel communication
*
*  Required Files:
*  ---------------
*  HttpMessage.cpp, HttpMessage.h, Utilities.h, Utilities.cpp
*
*  Public Interface:
*  --------------
*  parserHeader(const std::string & src)
*  swapURL()
*  parserCommand(HttpMessage & httpmsg)
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
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include "../Utilities/Utilities.h"

using namespace Utilities;
class HttpMessage 
{
public:
	using byte = char;
	using Name = std::string;
	using Value = std::string;
	using Attribute = std::pair<Name, Value>;
	using Attributes = std::vector<Attribute>;
	using Body = std::vector<byte>;

	void addAttribute(const Attribute& attrib);
	Value findValue(const Name& name);
	size_t findAttribute(const Name& name);
	bool removeAttribute(const Name& name);
	Attributes& attributes();
	static std::string attributeString(const Attribute& attrib);
	static Attribute attribute(const Name& name, const Value& value);
	static Attribute parserAtribute(const std::string src);

	// message body
	void setBody(byte buffer[], size_t buflen);
	size_t getBody(byte buffer[], size_t& buflen);
	void addBody(const Body& body);
	void addBody(const std::string& body);
	void addBody(size_t numBytes, byte* pBuffer);
	Body& body();
	size_t bodyLenth();

	//constrcut message
	static std::string getStrLine(size_t& pos, std::string str);
	static HttpMessage parserHeader(const std::string& src);
	static HttpMessage parserMessage(const std::string& src);
	static HttpMessage parserCommand(HttpMessage& httpmsg);

	//display
	std::string headerString() const;
	std::string bodyString() const;
	std::string toString() const;

	// get value of the message
	std::string command() { return _command; };	
	std::string localUrl() { return _localUrl; };
	std::string remoteUrl() { return _remoteUrl; };
	int localPort() { return _localPort; };
	int remotePort() { return _remotePort; };
	size_t fileSize() { return _fileSize; };
	std::string fileName() { return _fileName; };
	std::string filePath() { return _path; };

	// set value of the message
	void setCommand(const std::string& command);
	void setLocalUrl(const std::string& localUrl) { _localUrl = localUrl; };
	void setRemoteUrl(const std::string& remoteUrl) { _remoteUrl = remoteUrl; };
	void setLocalPort(const int& localPort) { _localPort = localPort; };
	void setRemotePort(const int& remotePort) { _remotePort = remotePort; };
	void setContent_length(const std::string length) { _content_length = Utilities::Converter<size_t>::toValue(length); };
	void setFileSize(int size) { _fileSize = size; };
	void setFileName(std::string name) { _fileName = name; };
	void setMode(std::string mode) { _mode = mode; };
	void setPath(std::string path) { _path = path; };

	size_t length() { return _length; };
	std::string contents() { return _contents; };

	// swap the from and to url
	void swapURL();
private:
	Attributes _attributes;
	Body _body;

	std::string _command;
	std::string _localUrl;
	std::string _remoteUrl;
	int _localPort = 0;
	int _remotePort = 0;
	std::string _mode;
	size_t _length = 0;
	std::string _contents;
	size_t _content_length = 0;

	std::string _fileName;
	size_t _fileSize = 0;
	std::string _path;
	std::string _writePath;


};

#endif