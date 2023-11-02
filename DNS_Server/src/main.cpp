#include <iostream>
#include <DNSMessage.h>
#include <DNSHeader.h>
#include <DNSQuestionSection.h>
#include <DNSResourceRecord.h>
#include <iomanip>

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
        .setQname("dns.google.com")
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
}
