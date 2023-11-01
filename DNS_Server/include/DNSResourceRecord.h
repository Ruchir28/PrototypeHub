#pragma once
#include <iostream>
#include <vector>
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

        friend class DNSResourceRecordBuilder;

    public: 
        std::vector<uint8_t> serialize() {
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
};

class DNSResourceRecordBuilder {
private:
    DNSResourceRecord rr;
public:
    DNSResourceRecordBuilder& setName(const std::string& name) {
        rr.name = name;
    }
    DNSResourceRecordBuilder& setType(uint16_t type) {
        rr.type = type;
    }
    DNSResourceRecordBuilder& setClass(uint16_t data_class) {
        rr.data_class = data_class;
    }
    DNSResourceRecordBuilder& setTTL(uint32_t ttl) {
        rr.ttl = ttl;
    }
    DNSResourceRecordBuilder& setRDLength(uint16_t data_length_in_octets) {
        rr.data_length_in_octets = data_length_in_octets;
    }
    DNSResourceRecordBuilder& setRData(const std::vector<uint8_t> rData) {
        rr.rdata = rData;
    }
    DNSResourceRecord build() {
        return rr;
    }
};
