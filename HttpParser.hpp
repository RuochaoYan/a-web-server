#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include <string>
#include <stdint.h>
#include <vector>

using namespace std;

class HttpParser {
public:
	static vector<string> parseRequest(string line);
    static string parseResponse(string key, string value);
};

#endif // HTTPPARSER_HPP
