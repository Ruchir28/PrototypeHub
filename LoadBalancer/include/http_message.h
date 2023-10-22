#include <string>
#include <unordered_map>

class HttpMessage {
public:
    HttpMessage();
    // For requests
    void setMethod(const std::string& method);
    void setPath(const std::string& path);
    void setVersion(const std::string& version);
    // For responses
    void setStatusCode(int code);
    void setStatusMessage(const std::string& message);
    // Common for both
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);

    std::string getMethod() const;
    std::string getPath() const;
    std::string getVersion() const;
    int getStatusCode() const;
    std::string getStatusMessage() const;
    std::string getHeader(const std::string& key) const;
    std::string getBody() const;

    void parse(const std::string& rawMessage);
    std::string serialize() const;

private:
    std::string method;
    std::string path;
    std::string version;
    int statusCode;
    std::string statusMessage;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};
