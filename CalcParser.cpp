#include <assert.h>
#include "CalcParser.hpp"
#include <sstream>
#include <iostream>
#include <string>
#include <utility>

using namespace std;

pair<string, string> CalcParser::parse(std::string insstr)
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
