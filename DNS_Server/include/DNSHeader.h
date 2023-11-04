#pragma once
#include <iostream>

class DNSHeader {
    private:
        uint16_t id; // identification number
        uint16_t flags; // flags
        uint16_t qdcount; // number of question entries
        uint16_t ancount;   // number of answer entries
        uint16_t nscount;   // number of authority entries
        uint16_t arcount;   // number of resource entries
        friend class DNSHeaderBuilder;
    public:
        std::vector<uint8_t> serializeHeader() const{
            std::vector<uint8_t> header;
            header.push_back(id >> 8);
            header.push_back(id & 0xFF);
            header.push_back(flags >> 8);
            header.push_back(flags & 0xFF);
            header.push_back(qdcount >> 8);
            header.push_back(qdcount & 0xFF);
            header.push_back(ancount >> 8);
            header.push_back(ancount & 0xFF);
            header.push_back(nscount >> 8);
            header.push_back(nscount & 0xFF);
            header.push_back(arcount >> 8);
            header.push_back(arcount & 0xFF);
            return header;
        }
        std::uint16_t getID() const {
            return id;
        }
        std::uint16_t getFlags() const {
            return flags;
        }
        std::uint16_t getQDCount() const {
            return qdcount;
        }
        std::uint16_t getANCount() const {
            return ancount;
        }
        std::uint16_t getNSCount() const {
            return nscount;
        }
        std::uint16_t getARCount() const {
            return arcount;
        }
        
        static DNSHeader parseHeader(const std::vector<uint8_t>& buffer, size_t& offset) {
            DNSHeader header;
            header.id = buffer[offset] << 8 | buffer[offset + 1];
            offset += 2;
            header.flags = buffer[offset] << 8 | buffer[offset + 1];
            offset += 2;
            header.qdcount = buffer[offset] << 8 | buffer[offset + 1];
            offset += 2;
            header.ancount = buffer[offset] << 8 | buffer[offset + 1];
            offset += 2;
            header.nscount = buffer[offset] << 8 | buffer[offset + 1];
            offset += 2;
            header.arcount = buffer[offset] << 8 | buffer[offset + 1];
            offset += 2;
            return header;
        }
};

class DNSHeaderBuilder {
    private:
        DNSHeader dnsHeader;
    public:
        DNSHeaderBuilder& setID(uint16_t id) { dnsHeader.id = id; return *this; }
        DNSHeaderBuilder& setFlags(uint16_t flags) { dnsHeader.flags = flags; return *this; }
        DNSHeaderBuilder& setQDCount(uint16_t qdCount) { dnsHeader.qdcount = qdCount; return *this; }
        DNSHeaderBuilder& setANCount(uint16_t anCount) { dnsHeader.ancount = anCount; return *this; }
        DNSHeaderBuilder& setNSCount(uint16_t nsCount) { dnsHeader.nscount = nsCount; return *this; }
        DNSHeaderBuilder& setARCount(uint16_t arCount) { dnsHeader.arcount = arCount; return *this; }
        DNSHeader build() const {return dnsHeader;}
};