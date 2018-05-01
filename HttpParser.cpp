#include <assert.h>
#include "HttpParser.hpp"
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// TO modify and impl

vector<string> HttpParser::parseRequest(string insstr)
{
    vector<string> v;
    stringstream ss(insstr);
    string item;
    getline(ss, item, ' ');
    v.push_back(item);
    getline(ss, item, ' ');
    v.push_back(item);
    if(v[0] == "GET"){
        getline(ss, item, ' ');
        v.push_back(item);
    }
    return v;
}

string HttpParser::parseResponse(string key, string value){
    return key + ' ' + value + "\r\n";
}
