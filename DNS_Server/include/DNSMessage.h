#pragma once
#include <iostream>
#include "DNSHeader.h"
#include "DNSQuestionSection.h"
#include "DNSResourceRecord.h"
class DNSMessage {
    private:
        DNSHeader header;
        std::vector<DNSQuestionSection> questions;
        std::vector<DNSResourceRecord> answers;
        std::vector<DNSResourceRecord> authorities;
        std::vector<DNSResourceRecord> additionals;
        friend class DNSMessageBuilder;
    public:
        // Getters
        DNSHeader getHeader() const {
            return header;
        }
        std::vector<DNSQuestionSection> getQuestions() const {
            return questions;
        }
        std::vector<DNSResourceRecord> getAnswers() const {
            return answers;
        }
        std::vector<DNSResourceRecord> getAuthorities() const {
            return authorities;
        }
        std::vector<DNSResourceRecord> getAdditionals() const {
            return additionals;
        }
};

class DNSMessageBuilder {
private:
    DNSHeader header;
    std::vector<DNSQuestionSection> questions;
    std::vector<DNSResourceRecord> answers;
    std::vector<DNSResourceRecord> authorities;
    std::vector<DNSResourceRecord> additionals;

public:
    DNSMessageBuilder& setHeader(const DNSHeader& header) {
        this->header = header;
        return *this;
    }

    DNSMessageBuilder& addQuestion(const DNSQuestionSection& question) {
        questions.push_back(question);
        return *this;
    }

    DNSMessageBuilder& addAnswer(const DNSResourceRecord& answer) {
        answers.push_back(answer);
        return *this;
    }

    DNSMessageBuilder& addAuthority(const DNSResourceRecord& authority) {
        authorities.push_back(authority);
        return *this;
    }

    DNSMessageBuilder& addAdditional(const DNSResourceRecord& additional) {
        additionals.push_back(additional);
        return *this;
    }

    DNSMessage build() {
        DNSMessage msg;
        msg.header = this->header;
        msg.questions = this->questions;
        msg.answers = this->answers;
        msg.authorities = this->authorities;
        msg.additionals = this->additionals;
        return msg;
    }
};
