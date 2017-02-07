///////////////////////////////////////////////////////////////////////
// SemiExp.cpp - collect tokens for analysis                         //
// ver 3.3                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
// Author:      Wenbin Li, Syracuse University                       //
//              wli102@syr.edu                                       //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides implment SemiExp class that collects and makes
* available sequences of tokens.  SemiExp uses the services of a Toker
* class to acquire tokens.  Each call to SemiExp::get() returns a
* sequence of tokens that ends in {.  This will be extended to use the
* full set of terminators: {, }, ;, and '\n' if the line begins with #.
*
* This is a new version for students in CSE687 - OOD, Spring 2016 to use
* for Project #1.
*
* Build Process:
* --------------
* Required Files:
*   SemiExpression.h, SemiExpression.cpp, Tokenizer.h, Tokenizer.cpp
*   ItokCollection.h
* Build Command: devenv Project1.sln/rebuild debug
* Run Command: Debug/SemiExp.exe
* Maintenance History:
* --------------------
* ver 3.3 : 10 Feb 2016
* Drived from ItokCollection class, and overrid get and show function
* ver 3.2 : 02 Feb 2016
* - declared SemiExp copy constructor and assignment operator = delete
* - added default argument for Toker pointer to nullptr so SemiExp
*   can be used like a container of tokens.
* - if pToker is nullptr then get() will throw logic_error exception
* ver 3.1 : 30 Jan 2016
* - changed namespace to Scanner
* - fixed bug in termination due to continually trying to read
*   past end of stream if last tokens didn't have a semiExp termination
*   character
* ver 3.0 : 29 Jan 2016
* - built in help session, Friday afternoon
*
* Planned Additions and Changes:
* ------------------------------
* - add public :, protected :, private : as terminators
* - move creation of tokenizer into semiExp constructor so
*   client doesn't have to write that code.
*/
#include <fstream>
#include <iostream>
#include <string>
#include <cctype>
#include <unordered_map>
#include <exception>
#include "SemiExp.h"
#include "../Tokenizer/Tokenizer.h"

using namespace Scanner;
/* SemiExp constructor */
SemiExp::SemiExp(Toker* pToker) : _pToker(pToker) {}
void SemiExp::setPtoker(Toker* pToker) {
	_pToker = pToker;
}

/* get the next semi expression */
bool SemiExp::get(bool clear)
{
	int forSemiCount = 0;
	int forBrace = 1;
	if (_pToker == nullptr)
		throw(std::logic_error("no Toker reference"));
	if (clear)
		_tokens.clear();
	while (true)
	{
		std::string token = _pToker->getTok();
		if (token == "")
			break;
		_tokens.push_back(token);

		if (isEndOfSemiExp(forBrace, forSemiCount, token, _tokens)) 
			return true;
	}
	trimFront();
	return false;
}

/* get the semiexpression's size */
size_t SemiExp::length() {
	return _tokens.size();
}

/* override the [] ooperator for SemiExp class */
std::string& SemiExp:: operator[](int n) {
	if (n < 0 || length() <= (size_t)n)
		throw std::exception("SemiExp index out of range");
	return _tokens[n];
}

/*--- is tok found in semi-expression? ---*/
size_t SemiExp::find(const std::string& tok)
{
	for (size_t i = 0; i < length(); ++i)
		if (tok == _tokens[i])
			return i;
	return length();
}

/* define the pushu_back function for SemiExp class */
void SemiExp::push_back(const std::string& tok)
{
	_tokens.push_back(tok);
}

/* remove the element according to name of element */
bool SemiExp::remove(const std::string& tok)
{
	std::vector<std::string>::iterator it;
	it = std::find(_tokens.begin(), _tokens.end(), tok);
	if (it != _tokens.end())
	{
		_tokens.erase(it);
		return true;
	}
	return false;
}

/* remove tok at specified indes */
bool SemiExp::remove(size_t i)
{
	if (i < 0 || _tokens.size() <= i)
		return false;
	std::vector<std::string>::iterator it = _tokens.begin();
	_tokens.erase(it + i);
	return true;
}

/* print out the lower case for characters */
void SemiExp::toLower()
{
	for (size_t i = 0; i < length(); ++i)
	{
		for (size_t j = 0; j < (*this)[i].length(); ++j)
		{
			char tmp;
			//(*this)[i][j] = tolower((*this)[i][j]);
			tmp = tolower((*this)[i][j]);
			(*this)[i][j] = tmp;
			std::cout << tmp;
		}
		std::cout << " ";
	}
}

/* trim the white space for SemiExp::get() */
void SemiExp::trimFront()
{
	while (_tokens.size() > 0 && (_tokens[0] == "\n" || _tokens[0] == " "))
		remove(0);
}

/* clear the vector of SemiExp class */
void SemiExp::clear() {
	_tokens.clear();
}

/* have not been implemented yet, just return true*/
bool SemiExp::merge(const std::string& firstTok, const std::string& secondTok) {
	return true;
}

/* --as for condtion judge the end character-- */
bool SemiExp::isEndOfSemiExp_for(int& forBrace, int& forSemiCount, std::string endStr, const std::vector<Token> __tokens) {
	if (endStr == "{" || endStr == "{ ")
		return true;
	if (endStr == "(")
		forBrace++;
	else if (endStr == ")")
		forBrace--;
	if ((endStr == ";" || endStr == "; ") && forBrace == 1)
		return true;
	else
		return false;
}

/* --as with # beginning condtion judge the end character-- */
bool SemiExp::isEndOfSemiExp_sharp(int& forBrace, int& forSemiCount, std::string endStr, const std::vector<Token> __tokens) {
	if (endStr == "\n")
		return true;
	else
		return false;
}

/* --as c++ comments and c comments condtion judge the end character-- */
bool SemiExp::isEndOfSemiExp_ccomment(int& forBrace, int& forSemiCount, std::string endStr, const std::vector<Token> __tokens) {
	if (endStr == "\n")
		return true;
	else
		return false;
}

/* judge if the character is termination for its semiexpression */
bool SemiExp::isEndOfSemiExp(int& forBrace, int& forSemiCount, std::string endStr, const std::vector<Token> __tokens) {
	if (__tokens[0] == "for") 
		return isEndOfSemiExp_for(forBrace, forSemiCount, endStr, __tokens);
	else if (endStr == ";" || endStr == "; ") {
		return true;
	}
	if (__tokens.size() >= 2)
		if ((__tokens[0] == "public" || __tokens[0] == "private") && (__tokens[1] == ":" || _tokens[1] == ": "))
			return true;
	if (__tokens[0] == "#")
		return isEndOfSemiExp_sharp(forBrace, forSemiCount, endStr, __tokens);
	if (__tokens[0].size() >= 2)
	{
		if (__tokens[0].at(0) == '/' && (__tokens[0].at(1) == '/' || __tokens[0].at(1) == '*'))
			return isEndOfSemiExp_ccomment(forBrace, forSemiCount, endStr, __tokens);
	}
	if (__tokens[0] == "\n")
		return true;
	if (endStr == "{" || endStr == "}" || endStr == "{ " || endStr == "} ")
		return true;
	else
		return false;
}

/* override the [] operator */
Token SemiExp::operator[](size_t n)
{
	if (n < 0 || n >= _tokens.size())
		throw(std::invalid_argument("index out of range"));
	return _tokens[n];
}


//void SemiExp::show()
//{
//  //std::cout << "\n  ";
//	if (_tokens.size() == 1 && _tokens[0] == "\n")
//		return;
//  for (auto token : _tokens)
//    if(token != "\n")
//      std::cout << token << " ";
//  std::cout << "\n";
//}

/* Show the Semi Expression. true showNewLines can show the empty line */
std::string SemiExp::show(bool showNewLines) {
	std::string tmp;
	bool alreadyShow = false;
	if (_tokens.size() == 1 && _tokens[0] == "\n") {
		if (showNewLines) {
			std::cout << "semiexpression-- newline";
			alreadyShow = true;
			_tokens[0] = " ";
		}	
		else
			return tmp;
	}
	if (!alreadyShow)
		std::cout << "semiexpression-- ";
	for (auto token : _tokens)
		if (token != "\n") { 
			std::cout << token << " ";
			tmp += token;
			tmp += " ";
		}
	std::cout << "\n";
	return tmp;
}

//----< test stub >--------------------------------------------------
#ifdef TEST_SEMIEXP
int main()
{
	Toker toker;
	std::string fileSpec = "../testBraceBug.cpp";
	std::fstream in(fileSpec);
	if (!in.good())
	{
		std::cout << "\n  can't open file " << fileSpec << "\n\n";
		return 1;
	}
	toker.attach(&in);
	toker.setSpecialPairChar("++");
	SemiExp semi(&toker);
	while (semi.get(true))
	{
		//std::cout << "\n  -- semiExpression --";
		semi.show();
	}
	/*
	   May have collected tokens, but reached end of stream
	   before finding SemiExp terminator.
	 */
	if (semi.length() > 0)
	{
		//std::cout << "\n  -- semiExpression --";
		semi.show(false);
		std::cout << "\n\n";
	}
	return 0;
}
#endif