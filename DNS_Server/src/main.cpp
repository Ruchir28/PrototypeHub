#include <iostream>
#include <DNSMessage.h>
#include <DNSHeader.h>
#include <DNSQuestionSection.h>
#include <DNSResourceRecord.h>
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main() {
    DNSHeaderBuilder dnsHeaderBuilder;
    dnsHeaderBuilder
        .setID(0x16)
        .setFlags(0x0100)
        .setQDCount(1)
        .setANCount(0)
        .setNSCount(0)
        .setARCount(0);
    DNSHeader dnsHeader = dnsHeaderBuilder.build();

    DNSQuestionSectionBuilder questionBuilder;

    questionBuilder
        .setQname("www.google.com")
        .setQtype(1)  // A record
        .setQclass(1);  // IN class
    DNSQuestionSection question = questionBuilder.build();

    for(auto byte : question.serialize()) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
    }
    std::cout << std::endl;

    DNSMessageBuilder messageBuilder;
    messageBuilder
        .setHeader(dnsHeader)
        .addQuestion(question);
    DNSMessage message = messageBuilder.build();

    std::vector<uint8_t> byteString = message.serialize();


    for(auto byte : byteString) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
    }
    std::cout << std::endl;

        // Create a UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error opening socket" << std::endl;
        return 1;
    }

    // Define the DNS server address and port
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(53);
    servaddr.sin_addr.s_addr = inet_addr("8.8.8.8"); // Google's public DNS server

    std::vector<uint8_t> query = byteString; 

    // Send the query to the DNS server
    if (sendto(sockfd, query.data(), query.size(), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Error in sendto" << std::endl;
        close(sockfd);
        return 1;
    }

    // Wait for the response
    std::vector<uint8_t> buffer(512);
    socklen_t len = sizeof(servaddr);
    ssize_t n = recvfrom(sockfd, buffer.data(), buffer.size(), 0, (struct sockaddr *)&servaddr, &len);
    if (n < 0) {
        std::cerr << "Error in recvfrom" << std::endl;
        close(sockfd);
        return 1;
    }

    std::cout << "Response:" << std::endl;

    for(int i = 0;i < n;i++) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(buffer[i]);
    }

    std::cout << std::endl;

    // parsing the respone 

    size_t offset = 0;
    DNSHeader header = DNSHeader::parseHeader(buffer, offset);
    std::cout << "ID: " << header.getID() << std::endl;
    std::cout << "ANSWER COUNT: " << header.getANCount() << std::endl;
    for(int i=0;i<header.getQDCount();i++) {
        DNSQuestionSection questionSection = DNSQuestionSection::parse(buffer, offset);
        std::cout << "QNAME: " << questionSection.getQname() << std::endl;
    }
    for(int i=0;i<header.getANCount();i++) {
        DNSResourceRecord answer = DNSResourceRecord::parse(buffer, offset);
        std::cout << "NAME: " << answer.getName() << std::endl;
        std::cout << "TYPE: " << answer.getType() << std::endl;
        std::cout << std::dec << "TTL:" << answer.getTTL() << std::endl;
        std::cout << "DATA LENGTH: " << answer.getDataLengthInOctets() << std::endl;
        std::cout << "IP ADDRESS: " << answer.getRdataDecoded() << std::endl;
        std::cout << "RDATA: ";
        for(auto byte : answer.getRdata()) {
            std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
        }
        std::cout << std::endl;
    }
    for(int i=0;i<header.getNSCount();i++) {
        DNSResourceRecord authority = DNSResourceRecord::parse(buffer, offset);
        std::cout << "NAME: " << authority.getName() << std::endl;
        std::cout << "TYPE: " << authority.getType() << std::endl;
        std::cout << "DATA LENGTH: " << authority.getDataLengthInOctets() << std::endl;
        std::cout << "RDATA: ";
        for(auto byte : authority.getRdata()) {
            std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
        }
        std::cout << std::endl;
    }

    for(int i=0;i<header.getARCount();i++) {
        DNSResourceRecord additional = DNSResourceRecord::parse(buffer, offset);
        std::cout << "NAME: " << additional.getName() << std::endl;
        std::cout << "TYPE: " << additional.getType() << std::endl;
        std::cout << "DATA LENGTH: " << additional.getDataLengthInOctets() << std::endl;
        std::cout << "RDATA: ";
        for(auto byte : additional.getRdata()) {
            std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
        }
        std::cout << std::endl;
    }

    close(sockfd);
    return 0;

}
