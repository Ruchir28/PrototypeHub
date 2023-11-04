#pragma once
#include <iostream>
#include <vector>
#include <sstream>
class DNSResourceRecord {
    private:
        /*
        A domain name to which this resource record pertains
        */
        std::string name;
        /**
         * Resource recprd type code
        */
        uint16_t type;
        /**
         * 2 octets which specify the class of data in RDATA field
        */
        uint16_t data_class;
        /**
         * TTL
        */
        uint32_t ttl;
        /**
         * Length of the variable lenth Resource Record i.e Data in octets
        */
        uint16_t data_length_in_octets;
        /**
         * Resource Record Data 
        */
        std::vector<uint8_t> rdata;
        /**
         * Decoded Resource Record Data
        */
       std::string rdata_decoded;

        friend class DNSResourceRecordBuilder;

    public: 
        enum RecordType {
            A = 1
        };
        std::vector<uint8_t> serialize() const {
            std::vector<uint8_t> rr;
            std::vector<std::string> labels;
            int start_index = 0;
            for(int i = 0;i < name.length();i++) {
                if(name[i] == '.') {
                    std::string label = name.substr(start_index,i - start_index);
                    start_index = i + 1;
                    labels.push_back(label);
                }
            }
            std::string last_label = name.substr(start_index);
            labels.push_back(last_label);
            for(std::string label: labels) {
                uint8_t len = static_cast<uint8_t>(label.length());  // Assuming length fits in 8 bits
                rr.push_back(len);
                for (char c : label) {
                    rr.push_back(static_cast<uint8_t>(c));
                }
            }
            rr.push_back(0);  // To mark the end of the domain name
            rr.push_back(type >> 8);
            rr.push_back(type & 0xFF);
            rr.push_back(data_class >> 8);
            rr.push_back(data_class & 0xFF);
            rr.push_back(ttl >> 24);
            rr.push_back((ttl >> 16) & 0xFF);
            rr.push_back((ttl >> 8) & 0xFF);
            rr.push_back(ttl & 0xFF);
            rr.push_back(data_length_in_octets >> 8);
            rr.push_back(data_length_in_octets & 0xFF);
            for(uint8_t byte: rdata) {
                rr.push_back(byte);
            }
            return rr;
        }

        static DNSResourceRecord parse(const std::vector<uint8_t>& buffer,size_t& offset) {
            std::string name;
            uint8_t pointer_mask = (1 << 7) | (1 << 6);
            bool contains_pointer = (buffer[offset] & pointer_mask) == pointer_mask;
            if(contains_pointer) {
                uint16_t pointer_offset = buffer[offset] & 0x3F;
                pointer_offset = (pointer_offset << 8) | buffer[offset + 1];
                offset += 2;
                // then read tje name from the pointer offset
                while(buffer[pointer_offset] != 0) {
                    uint8_t len = buffer[pointer_offset];
                    pointer_offset++;
                    for(int i=0;i<len;i++) {
                        name += static_cast<char>(buffer[pointer_offset]);
                        pointer_offset++;
                    }
                    if (buffer[pointer_offset] != 0) { // Check if it's the end of the qname
                        name += '.';
                    }
                }
            } else {
                while(buffer[offset] != 0) {
                    uint8_t len = buffer[offset];
                    offset++;
                    for(int i=0;i<len;i++) {
                        name += static_cast<char>(buffer[offset]);
                        offset++;
                    }
                    if (buffer[offset] != 0) { // Check if it's the end of the qname
                        name += '.';
                    }
                }
                offset++;
            }
            uint16_t type = buffer[offset] << 8 | buffer[offset + 1];
            offset += 2;
            uint16_t data_class = buffer[offset] << 8 | buffer[offset + 1];
            offset += 2;
            uint32_t ttl = buffer[offset] << 24 | buffer[offset + 1] << 16 | buffer[offset + 2] << 8 | buffer[offset + 3];
            offset += 4;
            uint16_t data_length_in_octets = buffer[offset] << 8 | buffer[offset + 1];
            offset += 2;
            std::vector<uint8_t> rdata;
            for(int i = 0;i < data_length_in_octets;i++) {
                rdata.push_back(buffer[offset]);
                offset++;
            }
            DNSResourceRecord rr;
            switch (type) 
            {
            case A: {
                std::ostringstream ip;
                ip << static_cast<int>(rdata[0]) << "." << static_cast<int>(rdata[1]) << "." << static_cast<int>(rdata[2]) << "." << static_cast<int>(rdata[3]);
                rr.rdata_decoded = ip.str();
                break;
            }
            default:
                break;
            }

            rr.name = name;
            rr.type = type;
            rr.data_class = data_class;
            rr.ttl = ttl;
            rr.data_length_in_octets = data_length_in_octets;
            rr.rdata = rdata;
            return rr;
        }

        std::string getName() const {
            return name;
        }

        uint16_t getType() const {
            return type;
        }

        uint16_t getClass() const {
            return data_class;
        }

        uint32_t getTTL() const {
            return ttl;
        }

        uint16_t getDataLengthInOctets() const {
            return data_length_in_octets;
        }

        std::vector<uint8_t> getRdata() const {
            return rdata;
        }

        std::string getRdataDecoded() const {
            return rdata_decoded;
        }

};

class DNSResourceRecordBuilder {
private:
    DNSResourceRecord rr;
public:
    DNSResourceRecordBuilder& setName(const std::string& name) {
        rr.name = name;
        return *this;
    }
    DNSResourceRecordBuilder& setType(uint16_t type) {
        rr.type = type;
        return *this;
    }
    DNSResourceRecordBuilder& setClass(uint16_t data_class) {
        rr.data_class = data_class;
        return *this;
    }
    DNSResourceRecordBuilder& setTTL(uint32_t ttl) {
        rr.ttl = ttl;
        return *this;
    }
    DNSResourceRecordBuilder& setRDLength(uint16_t data_length_in_octets) {
        rr.data_length_in_octets = data_length_in_octets;
        return *this;
    }
    DNSResourceRecordBuilder& setRData(const std::vector<uint8_t> rData) {
        rr.rdata = rData;
        return *this;
    }
    DNSResourceRecord build() {
        return rr;
    }
};
