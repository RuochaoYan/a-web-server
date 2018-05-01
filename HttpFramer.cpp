#include <assert.h>
#include "HttpFramer.hpp"
#include <queue>
#include <string>
#include <iostream>

// TO modify and impl

using namespace std;

void HttpFramer::append(string chars)
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

bool HttpFramer::hasMessage() const
{
	return !messages.empty();
}

string HttpFramer::topMessage() const
{
	return messages.front();
}

void HttpFramer::popMessage()
{
	messages.pop();
}

void HttpFramer::printToStream(ostream& stream) const
{
        queue<string> copy = messages;
	while (!copy.empty()) {
          cout << copy.front() << endl;
          copy.pop();
        }
}
