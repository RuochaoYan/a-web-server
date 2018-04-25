#ifndef CALCPARSER_HPP
#define CALCPARSER_HPP

#include <string>
#include <stdint.h>
#include <utility>

using namespace std;

class CalcParser {
public:
	static pair<string, string> parse(string insstr);
};

#endif // CALCPARSER_HPP
