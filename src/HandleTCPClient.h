#ifndef HANDLETCPCLIENT_H
#define HANDLETCPCLIENT_H

#include <string>
#include <map>
#define BUFSIZE 8192

using namespace std;

typedef struct HttpRequest_t {
    string method;
    string url;
    string version;
    map<string, string> headers;
} HttpRequest;

typedef struct HttpResponse_t {
    int status_code;
    string url;
    string version;
} HttpResponse;

void HandleTCPClient(int clntSocket, string doc_root);

#endif 



