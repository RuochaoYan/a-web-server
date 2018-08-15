
## Overview
This project is a simple webserver that implements a subset of the HTTP/1.1 protocol specification.

## Triton-HTTP/1.1 Specification for this project
In this project, we are going to be implementing a subset of the full HTTP/1.1 specification (which is many hundreds of pages long when you consider all the extensions and supplemental documents!). Because our implementation differs slightly from the official HTTP spec, we’re calling it “TritonHTTP.” Portions of this specification are courtesy of James Marshall, used with permission from the author. If you have any questions about what the spec is supposed to be, please refer to this document–don’t go to the actual HTTP spec because it is way more complex than Triton-HTTP.

### Client/server protocol
TritonHTTP is a client-server protocol that is layered on top of the reliable stream-oriented transport protocol TCP. Clients issue request messages to the server, and servers reply with response messages. In its most basic form, a single HTTP-level request-reply exchange happens over a single, dedicated TCP connection. The client first connects to the server, sends the HTTP request message, the server replies with an HTTP response, and then the server closes the connection:


Repeatedly setting up and tearing down TCP connections reduces overall network throughput and efficiency, and so HTTP has a mechanism whereby a client can reuse a TCP connection to a given server. The idea is that the client opens a TCP connection to the server, issues an HTTP request, gets an HTTP reply, and then issues another HTTP request on the already open outbound part of the connection. The server replies with the response, and this can continue through multiple request-reply interactions. The client signals the last request by setting a “Connection: close” header. The server indicates that it will not handle additional requests by setting the “Connection: close” header in the response. Note that the client can issue more than one HTTP request without necessarily waiting for full HTTP replies to be returned.


To support clients that do not properly set the “Connection: close” header, the server must implement a timeout mechanism to know when it should close the connection (otherwise it might just wait forever). For this project, you should set a server timeout of 5 seconds, meaning that if the server doesn’t receive a complete HTTP request from the client after 5 seconds, it sends back a “400” client error message and then closes the connection. We will discuss what a 400 client error message is below.

## HTTP messages
HTTP request and response messages are plain-text, consisting of a header section and an optional body section. The header section is separated from the body section with a blank line. The header consists of an initial line (which is different between requests and responses), followed by zero or more key-value pairs. Every line is terminated by a CRLF (carriage-return followed by a line feed). Thus a message looks like:

<initial line>[CRLF]
Key1: Value1[CRLF]
Key2: Value2[CRLF]
Key3: Value3[CRLF]
...
[CRLF]
<optional body...>
Messages without a body section still have the trailing CRLF (a blank line) present so that the server knows that it should not expect additional headers. HTTP requests and responses can be of arbitrary size.

### Request Initial Line
The initial line of an HTTP request header has three components:

The method (in this project that will be GET)
The URL
The highest HTTP version that the client supports
The method field indicates what kind of request the client is issuing. The most common is a GET request, which indicates that the client wants to download the content indicated by the URL (described next).

The URL is a pointer to the resource that the client is intersted in. Examples include /images/myimg.jpg and /classes/fall/cs101/index.html.

The version field takes the form HTTP/x.y, where x.y is the highest version that the client supports. For this course we’ll always use 1.1, so this value should be HTTP/1.1.

The fully formed inital request line would thus look something like:

GET /images/myimg.jpg HTTP/1.1
### Response Initial Line
The initial line of an HTTP response also has three components, which are slightly different than those in the request line:

The highest HTTP version that the server supports
A three-digit numeric code indicating the status of the request (e.g., whether it succeeded or failed, including more fine-grained information about how to interpret this response)
A human-friendly text description of the return code
HTTP Response code semantics
The first digit of the response code indicates the type of response. These types include:

1xx is informational
2xx is a success type
3xx means that the content the client is looking for is located somewhere else
4xx means that the client’s request had some kind of error in it
5xx means that the server encountered an error while trying to serve the client’s request
For this project, you’ll need to support:

200 OK: The request was successful
400 Client Error: The client sent a malformed or invalid request that the server doesn’t understand
403 Forbidden: The request was not served because the client wasn’t allowed to access the requested content
404 Not Found: The requested content wasn’t there
### HTTP header key-value pairs
After the intial request line, the HTTP message can optionally contain zero or more key-value pairs that add additional information about the request or response (called “HTTP Headers”). Some of the keys are specific to the request message, some are specific to response messages, and some can be used with both requests and responses. The format of these key-value pairs is inspired by RFC 822 (though again, don’t worry about the 822 spec–just go by what is here).

For this assignment, you must implement or support the following HTTP headers:

Request headers:

Host (required, 400 client error if not present)
Connection (optional, if set to “close” then server should close connection with the client after sending response for this request)
You should gracefully handle any other valid request headers that the client sends. Any request headers not in the proper form (e.g., missing a colon), should signal a 400 error.
Response headers:

Server (required)
Last-Modified (required only if return type is 200)
Content-Type (required if return type is 200; otherwise if you create a custom error page, you can set this to ‘text/html’)
Content-Length (required if return type is 200; otherwise if you create a custom error page, you can set this to the length of that response)
The format for the last-modified header is Last-Modified: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT.

The Content-Type for .jpg files should be “image/jpeg”, for .png files it should be “image/png”, and for html it should be “text/html”

A custom error page is simply a human-friendly message (formatted as HTML) explaining what went wrong in the case of an error; custom error pages are optional, however if you use one, the Content-Type and Content-Length headers have to be set correctly.

## Project details
### Basic web server functionality
At a high level, a web server listens for connections on a socket (bound to a specific adderss and port on a host machine). Clients connect to this socket and use the above-specified HTTP protocol to retrieve files from the server. For this project, your server will need to be able to serve out HTML files as well as images in jpg and png formats. You do not need to support server-side dynamic pages, Node.js, server-side CGI, etc.

### Mapping relative URLs to absolute file paths
Clients make requests to files using a Uniform Resource Locator, such as /images/cyrpto/enigma.jpg. One of the key things to keep in mind in building your web server is that the server must translate that relative URL into an absolute filename on the local filesystem. For example, you might decide to keep all the files for your server in ~aturing/cse101/server/www-files/, which we call the document root. When your server gets a request for the above-mentioned enigma.jpg file, it will prepend the document root to the specified file to get an absolute file name of ~aturing/cse101/server/www-files/images/crypto/enigma.jpg. You need to ensure that malformed or malicious URLs cannot “escape” your document root to access other files. For example, if a client submits the URL /images/../../../.ssh/id_dsa, they should not be able to download the ~aturing/.ssh/id_dsa file. If a client uses one or more .. directories in such a way that the server would “escape” the document root, you should return a 404 Not Found error back to the client. Take a look at the realpath() system call for help in dealing with document roots.

### Filesystem permissions
After your server maps the client’s request into a file in the document root, you must check to see whether that file actually exists and if the proper permissions are set on the file (the file has to be “world” readable). If the file does not exist, a file not found error (error code 404) is returned. If a file is present but the proper permissions are not set, a permission denied error is returned (error code 403). When a 403 error is returned, no information about the real file should be returned in the headers or body section of the reply (e.g., the file size). Otherwise, a 200 OK message is returned along with the contents of a file.

You should also note that web servers translate GET / to GET /index.html. That is, index.html is assumed to be the filename if no explicit filename is present. That is why the two URLs http://www.cs.ucsd.edu and http://www.cs.ucsd.edu/index.html return equivalent results. You will need to support this mapping in your server.

### Program structure
At a high level, your program will be structured as follows.

#### Initialize
We will provide you with starter code that handles command-line arguments, and will call into your C/C++ code with a port and the document root. Note that the document root and port number will be parameters that are passed into your program–do not hard code file paths or ports, as we will be testing your code against our own document root. Also do not assume that the files to serve out are in the same directory as the web server. We will call your program with an asbolute path to the document root that may or may not end in a final forward slash: e.g., “/var/home/htdocs” and/or “/var/home/htdocs/”.

#### Setup server socket and threading
Create a TCP server socket, and arrange so that a thread is spawned (or thread in a thread pool is retrieved) when a new connection comes in.

#### Separating framing from parsing
Your server should instantiate a stateful framing class, and a stateless parser class/function.

You will write code that reads from the client socket and pushes that data into your framer object, which returns HTTP messages. Your parser parses these messages into HTTP requests. Your server should handle the request, and generate a response, which your parser will convert into a message. Finally, your framing code will send this message over the socket back to the client. Your code must separate parsing and framing into separate steps to receive credit.

