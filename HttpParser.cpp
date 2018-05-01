#include <assert.h>
#include "HttpParser.hpp"
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

// TO modify and impl

pair<string, string> HttpParser::parse(std::string insstr)
{
        pair<string, string> p;

        if (insstr.size() == 0) {
          p.first = "end";
	}
        else {
	  stringstream ss(insstr);
          string item;
          getline(ss, item, ' ');
          p.first = item;
          getline(ss, item, ' ');
          p.second = item;
        }

	return p;
}
