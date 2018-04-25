#include <assert.h>
#include "CalcFramer.hpp"
#include <queue>
#include <string>
#include <iostream>

using namespace std;

void CalcFramer::append(string chars)
{
        s.append(chars);
	size_t start = 0;
	size_t i = 0;
        for (; i < s.size(); i++) {
          if (s[i] == '\r' && i < s.size() - 1 && s[i + 1] == '\n'){
            messages.push(s.substr(start, i - start));
            start = i + 2;
            i++;
          }
        }
        s = s.substr(start, s.size() - start);
}

bool CalcFramer::hasMessage() const
{
	return !messages.empty();
}

string CalcFramer::topMessage() const
{
	return messages.front();
}

void CalcFramer::popMessage()
{
	messages.pop();
}

void CalcFramer::printToStream(ostream& stream) const
{
        queue<string> copy = messages;
	while (!copy.empty()) {
          cout << copy.front() << endl;
          copy.pop();
        }
}
