/////////////////////////////////////////////////////////////////////
// HttpMessage.cpp - HttpMessage for channel communicatioin        //
//                 - for CSE 687 Project #4                        //
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
#include "HttpMessage.h"
#include "../Utilities/Utilities.h"
#include <iostream>

using namespace Utilities;
using Attribute = HttpMessage::Attribute;
using Attributes = HttpMessage::Attributes;
using Name = HttpMessage::Name;
using Value = HttpMessage::Value;
using Body = HttpMessage::Body;
using byte = HttpMessage::byte;

class MockSocket {
public:
	MockSocket(const HttpMessage& msg) : _msgString(msg.toString()) {};
	std::string recvString();
	bool recv(size_t bytes, byte* buffer);
	size_t& getConten_length() { return _content_length; };
private:
	std::string _msgString;
	size_t _pos = 0;
	size_t _content_length = 0;
};

//recv a line of message
std::string MockSocket::recvString() {
	std::string line;
	char ch;
	while ((ch = _msgString[_pos++]) != '\n')
	{
		line += ch;
	}
	return line;
}

// get the buffer 
bool MockSocket::recv(size_t bytes, byte * buffer)
{
	for (size_t i = 0; i < bytes; i++)
	{
		if (_pos + i == _msgString.size())
		{
			std::cout << "\n----- attempt to index past end of msgString_ -----\n";
			return false;
		}
		buffer[i] = _msgString[_pos + i];
	}
	return true;
}

// push the attrib in _attributes
void HttpMessage::addAttribute(const Attribute& attrib)
{
	_attributes.push_back(attrib);
}

// find pos of element named name in _attributes
size_t HttpMessage::findAttribute(const Name& name)
{
	for (size_t i = 0; i < _attributes.size(); i++)
	{
		if (_attributes[i].first == name)
		{
			return i;
		}
	}
	return _attributes.size();
}

//find the value in attributes by specified name
Value HttpMessage::findValue(const Name& name)
{
	Value val = "";
	size_t pos = findAttribute(name);
	if (pos < _attributes.size())
	{
		val = _attributes[pos].second;
	}
	return val;
}

// remove the attribute by specified name
bool HttpMessage::removeAttribute(const Name& name)
{
	size_t pos = findAttribute(name);
	if (pos == _attributes.size())
	{
		return false;
	}
	std::vector<Attribute>::iterator iter = _attributes.begin() + pos;
	_attributes.erase(iter);
	return true;
}

// get collection of attirbute
Attributes & HttpMessage::attributes()
{
	return _attributes;
}

// parser the attributes
Attribute HttpMessage::parserAtribute(const std::string src)
{
	Attribute attrib("", "");
	size_t pos = src.find(':');
	if (pos == std::string::npos)
		return attrib;
	std::string first = StringHelper::trim(src.substr(0, pos));
	std::string second = StringHelper::trim(src.substr((pos + 1), (src.size() - 1)));
	attrib.first = first;
	attrib.second = second;
	return attrib;
}

// fill body from buffer
void HttpMessage::setBody(byte buffer[], size_t buflen)
{
	for (size_t i = 0; i < buflen; i++)
	{
		_body.push_back(buffer[i]);
	}
}

// fil buffer from body
/*
* - retunr length of buffer contents
* - return value is zero if buffer is too small to hold body
*/
size_t HttpMessage::getBody(byte buffer[], size_t & buflen)
{
	if (buflen < _body.size())
		return 0;
	size_t length = _body.size();
	for (size_t i = 0; i < length; i++)
		buffer[i] = _body[i];
	return length;
}

// replace the body with another body
void HttpMessage::addBody(const Body & body)
{
	_body = body;
}

/* replace the body with contents of string */
void HttpMessage::addBody(const std::string & body)
{
	_body.clear();
	for (auto ch : body)
		_body.push_back(ch);
}

/* replace the body from buffer contents */
void HttpMessage::addBody(size_t numBytes, byte * pBuffer)
{
	_body.clear();
	for (size_t i = 0; i < numBytes; i++)
		_body.push_back(*(pBuffer + i));
}

// get body
Body & HttpMessage::body()
{
	return _body;
}

// get length of body in bytes
size_t HttpMessage::bodyLenth()
{
	return _body.size()*sizeof(byte);
}

/* get a line of received message */
std::string HttpMessage::getStrLine(size_t& pos, std::string str) {
	std::string line;
	char ch;
	while ((ch = str[pos++]) != '\n'&& pos < str.length())
	{
		line += ch;
	}
	return line;
}

/* parser the header msg from string to element style */
HttpMessage HttpMessage::parserHeader(const std::string & src) {
	size_t _pos = 0;
	HttpMessage httpmsg;
	while (true)    //get header 
	{
		std::string line = getStrLine(_pos, src);
		if (line.size() == 0)
			break;
		Attribute attrib = HttpMessage::parserAtribute(line);
		httpmsg.addAttribute(attrib);
		if (attrib.first == "Command")
			httpmsg.setCommand(attrib.second);
		else if (attrib.first == "Content_length")
			httpmsg.setContent_length(attrib.second);
		else if (attrib.first == "FromAddr") {
			auto Fromurl = httpmsg.findValue("FromAddr");
			auto fromurl = httpmsg.parserAtribute(Fromurl);
			auto fromIP = fromurl.first;
			auto fromPort = fromurl.second;
			int intFromPort = Utilities::Converter<int>::toValue(fromPort);
			httpmsg.setLocalUrl(fromIP);
			httpmsg.setLocalPort(intFromPort);
		}
		else if (attrib.first == "ToAddr") {
			auto Tourl = httpmsg.findValue("ToAddr");
			auto tourl = httpmsg.parserAtribute(Tourl);
			auto toIP = tourl.first;
			auto toPort = tourl.second;
			int intToPort = Utilities::Converter<int>::toValue(toPort);
			httpmsg.setRemoteUrl(toIP);
			httpmsg.setRemotePort(intToPort);
		}
		else if (attrib.first == "FileSize") {
			auto FileSize = httpmsg.findValue("FileSize");
			auto fileSize = Converter<int>::toValue(FileSize);
			httpmsg.setFileSize(fileSize);
			httpmsg.setMode("OneWay");
		}
		else if (attrib.first == "Path") {
			auto FilePath = httpmsg.findValue("Path");
			httpmsg.setPath(FilePath);
		}
	}
	auto FileName = httpmsg.findValue("FileName");
	httpmsg.setFileName(FileName);
	auto end = src.length();
	auto bdy = src.substr(_pos, src.length());
	httpmsg.addBody(src.substr(_pos, src.length()));
	return httpmsg;
}
HttpMessage HttpMessage::parserMessage(const std::string & src)
{
	while (true)
	{
		size_t _pos = 0;
		std::string line = getStrLine(_pos, src);
		if (line.size() == 0)
			break;
	}
	return HttpMessage();
}
/* parser command infor in http msg */
HttpMessage HttpMessage::parserCommand(HttpMessage & httpmsg)
{
	httpmsg.setCommand(httpmsg.findValue("Command"));
	return httpmsg;
}

// conver the attribute pair to string
std::string HttpMessage::attributeString(const Attribute & attrib)
{
	return attrib.first + ":" + attrib.second + "\n";
}

// build attribute from name and value
Attribute HttpMessage::attribute(const Name & name, const Value & value)
{
	return std::pair<Name, Value>(name, value);
}

// convert message header to string
std::string HttpMessage::headerString() const
{
	std::string header;
	for (auto attrib : _attributes)
	{
		header += attributeString(attrib);
	}
	header += "\n";
	return header;
}

// convert body to string
std::string HttpMessage::bodyString() const
{
	std::string body;
	for (auto ch : _body)
		body += ch;
	return body;
}

// convert message to string
std::string HttpMessage::toString() const
{
	return headerString() + bodyString();
}
/* set command */
void HttpMessage::setCommand(const std::string & command)
{
	_command = command;
	removeAttribute("Command");
	std::string cmdstr = "Command";
	addAttribute(HttpMessage::Attribute(cmdstr, command));
}
/* swap url */
void HttpMessage::swapURL()
{
	std::string tmpAddr;
	int tmpPort;
	tmpAddr = _localUrl;
	_localUrl = _remoteUrl;
	_remoteUrl = tmpAddr;

	tmpPort = _localPort;
	_localPort = _remotePort;
	_remotePort = tmpPort;

	std::string toUrl = _remoteUrl + ":" + Converter<int>::toString(_remotePort);
	removeAttribute("ToAddr");
	addAttribute(HttpMessage::attribute("ToAddr", toUrl));

	std::string fromUrl = _localUrl + ":" + Converter<int>::toString(_localPort);
	removeAttribute("FromAddr");
	addAttribute(HttpMessage::attribute("FromAddr", fromUrl));
}

#ifdef TEST_HTTPMESSAGE

int main() {
	Utilities::StringHelper::Title("Test HttpMessage Package", '=');
	HttpMessage msg;
	msg.addAttribute(HttpMessage::attribute("Command", "GetFiles"));
	msg.addAttribute(HttpMessage::attribute("ToAddr", "127.0.0.1:8080"));
	msg.addAttribute(HttpMessage::attribute("FromAddr", "127.0.0.1:8081"));
	msg.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	msg.addAttribute(HttpMessage::attribute("Content_length", "10"));

	msg.addBody(std::vector<HttpMessage::byte> {'0', '1', '2', '3', '4', '5'});
	std::cout << "\n" << msg.toString() << "\n";

	StringHelper::title("testing headerString() bodySting(), and toString()");
	std::cout << "\nheader String:";
	std::cout << "\n" << msg.headerString();
	std::cout << "\nbody String:";
	std::cout << "\n" << msg.bodyString();
	std::cout << "\n\nmessage String:";
	std::cout << "\n" << msg.toString();

	StringHelper::title("testing removeAttribute");
	putline();
	msg.removeAttribute("Content_length");
	std::cout << msg.toString();
	putline();

	StringHelper::title("Testing addbody(const std::string&)");
	std::string msgBody = "<msg>this is a message</msg>";
	msg.addAttribute(HttpMessage::Attribute("Content_length", Converter<size_t>::toString(msgBody.size())));
	msg.addBody(msgBody);
	std::cout << msg.toString();
	putline();

	StringHelper::title("Testing parserAttribute(const std::string&)");
	std::string test2 = "name:value";
	std::cout << "\nInput = \"" << test2 << "\"";
	Attribute attrib2 = HttpMessage::parserAtribute(test2);
	std::cout << "\n result is: " << HttpMessage::attributeString(attrib2);
	test2 = " name : value ";
	std::cout << "\nInput = \"" << test2 << "\"";
	attrib2 = HttpMessage::parserAtribute(test2);
	std::cout << "\n result is: " << HttpMessage::attributeString(attrib2);

	StringHelper::title("Testing message parsering");
	MockSocket sock(msg);
	HttpMessage msg2;
	while (true)
	{
		std::string line = sock.recvString();
		if (line.size() == 0)
			break;
		Attribute attrib = HttpMessage::parserAtribute(line);
		msg2.addAttribute(attrib);
	}
	Value val = msg2.findValue("Content_length");
	if (val.size() > 0)
	{
		size_t numBytes = Converter<size_t>::toValue(val);
		byte* pBuffer = new byte[numBytes];
		sock.recv(numBytes, pBuffer);
		msg2.addBody(numBytes, pBuffer);
	}
	std::cout << "\n" << msg2.toString();
	putline();
}

#endif

