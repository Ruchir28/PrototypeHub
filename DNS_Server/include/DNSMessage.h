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
        std::vector<uint8_t> serialize() const {
            std::vector<uint8_t> message;
            std::vector<uint8_t> header = this->header.serializeHeader();
            message.insert(message.end(),header.begin(),header.end());
            // note: the number of questions,answers,authorities and additionals are all specified in header
            for(const DNSQuestionSection& question: questions) {
                std::vector<uint8_t> s_question = question.serialize();
                message.insert(message.end(),s_question.begin(),s_question.end());
            }
            for(const DNSResourceRecord& answer: answers) {
                std::vector<uint8_t> s_answer = answer.serialize();
                message.insert(message.end(),s_answer.begin(),s_answer.end());
            }
            for(const DNSResourceRecord& authority: authorities) {
                std::vector<uint8_t> s_authority = authority.serialize();
                message.insert(message.end(),s_authority.begin(),s_authority.end());
            }
            for(const DNSResourceRecord& additional: additionals) {
                std::vector<uint8_t> s_additional = additional.serialize();
                message.insert(message.end(),s_additional.begin(),s_additional.end());
            }
            return message;
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
