#include <assert.h>
#include "HttpFramer.hpp"
#include <queue>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h> /* for recv() and send() */

// TO modify and impl

using namespace std;

void HttpFramer::append(string chars)
{
        request.append(chars);
	size_t start = 0;
	size_t i = 0;
        for (; i < request.size(); i++) {
          if (request[i] == '\r' && i < request.size() - 1 && request[i + 1] == '\n'){
            messages.push(request.substr(start, i - start));
            start = i + 2;
            i++;
          }
        }
        request = request.substr(start, request.size() - start);
}

bool HttpFramer::hasMessage() const
{
	return !messages.empty();
}

string HttpFramer::topMessage() const
{
	return messages.front();
}

void HttpFramer::popMessage()
{
	messages.pop();
}

//void HttpFramer::printToStream(ostream& stream) const
//{
//        queue<string> copy = messages;
//    while (!copy.empty()) {
//          cout << copy.front() << endl;
//          copy.pop();
//        }
//}

void HttpFramer::addResponse(string line){
    response += line;
}

void HttpFramer::sendResponse(string url, int socket, bool fileExist){
    if(fileExist){
        ifstream inFile;
        inFile.open(url);//open the input file
        stringstream strStream;
        strStream << inFile.rdbuf();//read the file
        string str = strStream.str();//str holds the content of the file
        response += str;
    }
    send(socket, response.c_str(), response.size(), 0);
}
