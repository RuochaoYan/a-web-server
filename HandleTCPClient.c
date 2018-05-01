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


void HandleTCPClient(int clntSocket, string doc_root)
{
    map<string, string> request;
    char buffer[BUFSIZE];
    ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFSIZE - 1, 0);
    if (numBytesRcvd < 0)
      DieWithError("recv() failed");

    buffer[numBytesRcvd] = '\0';
    string s;
    HttpFramer framer;
    string res;
    s = buffer;

    while (numBytesRcvd > 0) {
      framer.append(s);
      while (framer.hasMessage()) {
        string line = framer.topMessage();
        framer.popMessage();
        pair<string, string> p = HttpParser::parse(line);
        request[p.first] = p.second;
      }
      numBytesRcvd = recv(clntSocket, buffer, BUFSIZE - 1, 0);
      if (numBytesRcvd < 0)
        DieWithError("recv() failed");
      buffer[numBytesRcvd] = '\0';
      s = buffer;
    }
  close(clntSocket);    /* Close client socket */
  for (auto it = request.cbegin(); it != request.cend(); it++) {
    cout << it->first << " " << it->second << endl;
  }
}
