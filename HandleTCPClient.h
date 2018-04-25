#ifndef HANDLETCPCLIENT_H
#define HANDLETCPCLIENT_H

#include <string>

#define BUFSIZE 8192

using namespace std;

void HandleTCPClient(int clntSocket, string doc_root);

#endif 



