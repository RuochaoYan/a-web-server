#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#include <iostream>
#include <assert.h>
#include "HttpFramer.hpp"
#include "HttpParser.hpp"
#include "DieWithError.h"
#include "HandleTCPClient.h"
#include <string>
#include <map>
#include <utility>
#include <fstream>
#include <sys/stat.h>
#include <ctime>

using namespace std;

void prepareResponse(int socket, HttpResponse response, HttpFramer framer){
    // the inital line of the response
    string status_code_message;
    switch(response.status_code){
        case 200:
            status_code_message = "200 OK";
            break;
        case 400:
            status_code_message = "400 Client Error";
            break;
        case 403:
            status_code_message = "403 Forbbiden";
            break;
        case 404:
            status_code_message = "404 Not Found";
            break;
    }
    string initial = HttpParser::parseResponse(response.version, status_code_message);
    framer.addResponse(initial);
    
    // the headers of the response
    // server
    framer.addResponse("Server: Potato1.0\r\n");
    bool fileExists = response.status_code == 200 ? true : false;
    if(fileExists){
        // last modified date
        // Last-Modified: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT.
        string mod_str;
        struct stat statRes;
        if(stat(response.url.c_str(), &statRes)==0)
        {
            time_t mod_time = statRes.st_mtime;
            struct tm *gm;
            gm = gmtime(&mod_time);
            char tmpbuf[128];
            strftime(tmpbuf, 128, "%A, %d %b %Y %T %Z\0", gm);
            mod_str = string(tmpbuf);
        }
        framer.addResponse(HttpParser::parseResponse("Last-Modified:", mod_str));
        
        // content type
        string extension = response.url.substr(response.url.find_last_of(".") + 1);
        string type;
        if(extension == "html")
            type = "text/html";
        else if(extension == "jpg")
            type = "image/jpeg";
        else if(extension == "png")
            type = "image/png";
        else
            type = "unknown type";
        framer.addResponse(HttpParser::parseResponse("Content-Type:", type));
        
        // content length
        ifstream file( response.url, ios::binary | ios::ate);
        int fileSize = (int)file.tellg();
        framer.addResponse(HttpParser::parseResponse("Content-Length:", to_string(fileSize)));
    }

    // add one more CRLF
    framer.addResponse("\r\n");
    
    // send the response message and/or the file to the client
    framer.sendResponse(response.url, socket, fileExists);
}

// check if a file exists
bool is_file_exist(const char *fileName)
{
    ifstream infile(fileName);
    return infile.good();
}

// check if the file is world readable
bool worldReadable(char const *file){
    struct stat st;
    bool res = false;
    if(stat(file, &st) == 0){
        mode_t perm = st.st_mode;
        res = (perm & S_IROTH) ? true : false;
        return res;
    }
    else{
        return res;
    }
}

void HandleTCPClient(int clntSocket, string doc_root)
{
    char buffer[BUFSIZE];
    ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFSIZE - 1, 0);
    if (numBytesRcvd < 0)
      DieWithError("recv() failed");

    buffer[numBytesRcvd] = '\0';
    string s;
    HttpFramer framer;
    string res;
    s = buffer;
    
    HttpRequest req;

    while (numBytesRcvd > 0) {
      framer.append(s);
        bool hasFinished = false;
      while (framer.hasMessage()) {
        string line = framer.topMessage();
        framer.popMessage();
          if(line.size() == 0){
              hasFinished = true;
              break;
          }
        vector<string> v = HttpParser::parseRequest(line);
        if(v.size() == 3){
          req.method = v[0];
          req.url = v[1];
          req.version = v[2];
        }
        else if(v.size() == 2){
          req.headers[v[0]] = v[1];
        }
        else{
          DieWithError("Wrong request message.");
        }
      }
      if(hasFinished)
          break;
      numBytesRcvd = recv(clntSocket, buffer, BUFSIZE - 1, 0);
      if (numBytesRcvd < 0)
        DieWithError("recv() failed");
      buffer[numBytesRcvd] = '\0';
      s = buffer;

    }
    // translate "GET /" to "GET /index.html"
    if(req.url == "/")
        req.url += "index.html";
    
    // convert relative path to absolute path
    string beforeConvert = doc_root + req.url;
    char afterConvert[80];
    realpath(beforeConvert.c_str(), afterConvert);

    // define "response"
    HttpResponse response;
    response.url = string(afterConvert);
    response.version = req.version;
    
    // check if the headers are valid - 400
    for(auto it = req.headers.cbegin(); it != req.headers.cend(); it++){
        if(it->first[it->first.size()-1] != ':'){
            response.status_code = 400;
            prepareResponse(clntSocket, response, framer);
        }
    }
    
    // other status code
    if(response.status_code != 400){
        // check if it is a malformed url - 404
        if(response.url.compare(0, doc_root.size(), doc_root)){
            response.status_code = 404;
        }
        
        // check if the file exists - 404
        else if(!is_file_exist(response.url.c_str())){
            response.status_code = 404;
        }
        
        // check if the file is world readable - 403
        else  if(!worldReadable(response.url.c_str())){
            response.status_code = 403;
        }
        
        // successful - 200
        else{
            response.status_code = 200;
        }
        prepareResponse(clntSocket, response, framer);
    }

//    if(req.headers["Connection:"] == "close")
    close(clntSocket);    /* Close client socket */
}
