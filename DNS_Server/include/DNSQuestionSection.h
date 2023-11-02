#pragma once
#include <iostream>

/**
 * @brief The QuestionSection of a DNS Message
 * refer: https://datatracker.ietf.org/doc/html/rfc1035#section-4.1.2
*/
class DNSQuestionSection {
    private:
        /**
            a domain name represented as a sequence of labels, where
            each label consists of a length octet followed by that
            number of octets.  The domain name terminates with the
            zero length octet for the null label of the root.  Note
            that this field may be an odd number of octets; no
            padding is used.
            @note: sequence of lables means part of domain seprated by dots i.e www.google.com -> labels are www,google,com
        */
        std::string qname;
        uint16_t qtype;
        uint16_t qclass;
        friend class DNSQuestionSectionBuilder;
        DNSQuestionSection(std::string qname,u_int16_t qtype,u_int16_t qclass) {
            this->qclass = qclass;
            this->qname = qname;
            this->qtype = qtype;
        }
    public:
        std::string getQname() const {
            return qname;
        }
        uint16_t getQtype() const {
            return qtype;
        }
        uint16_t getQclass() const {
            return qclass;
        }
        std::vector<uint8_t> serialize() const {
            std::vector<uint8_t> qsection;
            std::vector<std::string> labels;
            int start_index = 0;
            for(int i = 0;i < qname.length();i++) {
                if(qname[i] == '.') {
                    std::string label = qname.substr(start_index,i - start_index);
                    start_index = i + 1;
                    labels.push_back(label);
                }
            }
            std::string last_label = qname.substr(start_index);
            labels.push_back(last_label);
            for(std::string label: labels) {
                uint8_t len = static_cast<uint8_t>(label.length());  // Assuming length fits in 8 bits
                qsection.push_back(len);
                for (char c : label) {
                    qsection.push_back(static_cast<uint8_t>(c));
                }
            }
            qsection.push_back(0);  // To mark the end of the domain name
            qsection.push_back(qtype >> 8);
            qsection.push_back(qtype & 0xFF);
            qsection.push_back(qclass >> 8);
            qsection.push_back(qclass & 0xFF);
            return qsection;
        }
};

class DNSQuestionSectionBuilder {
    private:
        std::string qname;
        uint16_t qtype;
        uint16_t qclass;
    public:
        DNSQuestionSectionBuilder& setQname(std::string qname) {
            this->qname = qname;
            return *this;
        }
        DNSQuestionSectionBuilder& setQtype(uint16_t qtype) {
            this->qtype = qtype;
            return *this;
        }
        DNSQuestionSectionBuilder& setQclass(uint16_t qclass) {
            this->qclass = qclass;
            return *this;
        }
        DNSQuestionSection build() {
            return DNSQuestionSection(qname, qtype, qclass);
        }
};
