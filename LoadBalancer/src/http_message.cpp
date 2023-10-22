#include "http_message.h"

HttpMessage::HttpMessage() : statusCode(0) {}

std::vector<std::string> splitString(std::string str, std::string delimiter) {
    std::vector<std::string> result;
    int index = str.find(delimiter);
    while (index != std::string::npos) {
        result.push_back(str.substr(0, index));
        str = str.substr(index + delimiter.length());
        index = str.find(delimiter);
    }
    result.push_back(str);
    return result;
}

// For Requests
void HttpMessage::setMethod(const std::string& method) {
    this->method = method;
}

void HttpMessage::setPath(const std::string& path) {
    this->path = path;
}

void HttpMessage::setVersion(const std::string& version) {
    this->version = version;
}

// For Responses
void HttpMessage::setStatusCode(int code) {
    this->statusCode = code;
}

void HttpMessage::setStatusMessage(const std::string& message) {
    this->statusMessage = message;
}

// Common for both
void HttpMessage::setHeader(const std::string& key, const std::string& value) {
    this->headers[key] = value;
}

void HttpMessage::setBody(const std::string& body) {
    this->body = body;
}

// Getters
std::string HttpMessage::getMethod() const {
    return this->method;
}

std::string HttpMessage::getPath() const {
    return this->path;
}

std::string HttpMessage::getVersion() const {
    return this->version;
}

int HttpMessage::getStatusCode() const {
    return this->statusCode;
}

std::string HttpMessage::getStatusMessage() const {
    return this->statusMessage;
}

std::string HttpMessage::getHeader(const std::string& key) const {
    if (this->headers.find(key) != this->headers.end()) {
        return this->headers.at(key);
    }
    return "";
}

std::string HttpMessage::getBody() const {
    return this->body;
}


void HttpMessage::parse(const std::string& rawMessage) {
    // Split header and body
    int index = rawMessage.find("\r\n\r\n"); // headers and body seprated by empty line
    std::string header = rawMessage.substr(0, index);
    // skipping the new line character
    std::string body = rawMessage.substr(index + 4);

    std::vector<std::string> headerLines = splitString(header, "\r\n");
    std::vector<std::string> startLine = splitString(headerLines[0], " ");
    if(startLine[0] == "GET" || startLine[0] == "POST" || startLine[0] == "PUT" || startLine[0] == "DELETE"){
        // REQUEST 
        this->setMethod(startLine[0]);
        this->setPath(startLine[1]);
        this->setVersion(startLine[2]);
    } else {
        // RESPONSE
        this->setVersion(startLine[0]);
        this->setStatusCode(std::stoi(startLine[1]));
        this->setStatusMessage(startLine[2]);
    }
    // Headers
    for (int i = 1; i < headerLines.size(); i++) {
        std::vector<std::string> header = splitString(headerLines[i], ": ");
        this->setHeader(header[0], header[1]);
    }
    // Body
    this->setBody(body);
}

std::string HttpMessage::serialize() const {
    std::string result = "";
    if (this->method != "") {
        // Request
        result += this->method + " " + this->path + " " + this->version + "\r\n";
    } else {
        // Response
        result += this->version + " " + std::to_string(this->statusCode) + " " + this->statusMessage + "\r\n";
    }
    // Headers
    for (auto it = this->headers.begin(); it != this->headers.end(); it++) {
        result += it->first + ": " + it->second + "\r\n";
    }
    // Body
    result += "\r\n" + this->body;
    return result;
}

