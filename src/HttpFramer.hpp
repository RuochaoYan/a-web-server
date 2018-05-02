#ifndef HTTPFRAMER_HPP
#define HTTPFRAMER_HPP

#include <iostream>
#include <queue>
#include <string>

using namespace std;

class HttpFramer {
public:
	void append(std::string chars);

	// Does this buffer contain at least one complete message?
	bool hasMessage() const;

	// Copy the first instruction in this buffer into the provided class
	std::string topMessage() const;

	// Pops off the first instruction from the buffer
	void popMessage();

	// prints the string to an ostream (useful for debugging)
	void printToStream(std::ostream& stream) const;

	// a line of response header
	void addResponse(string line);

	// send header and/or file
        void sendResponse(string url, int socket, bool fileExists);

protected:
	queue<std::string> messages;
        string request;
        string response;
};

#endif // HTTPFRAMER_HPP
