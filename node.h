#pragma once

#include <iostream>
#include <memory>
#include <stack>
#include <vector>
#include <string>
#include <cstdint>

#include "date.h"

using namespace std;

enum LogicalOperation {
    And, Or
};

enum Comparison {
    Less, LessOrEqual, Greater, GreaterOrEqual, Equal, NotEqual
};

struct Node {
    virtual bool Evaluate(const Date &date, const std::string &event) const = 0;
};

struct EmptyNode : public Node {
    bool Evaluate(const Date &date, const std::string &event) const override;
};

struct LogicalOperationNode : public Node {
public:
    LogicalOperationNode(LogicalOperation operation, shared_ptr<Node> left,
                         shared_ptr<Node> right);

    bool Evaluate(const Date &date, const std::string &event) const override;

private:
    shared_ptr<Node> left;
    shared_ptr<Node> right;
    LogicalOperation operation;
};

struct DateComparisonNode : public Node {
public:
    DateComparisonNode(const Comparison &comparison, const Date &date);

    bool Evaluate(const Date &date, const std::string &event) const override;

private:
    Comparison comparison;
    Date date;
};

struct EventComparisonNode : public Node {
public:
    EventComparisonNode(const Comparison &comparison, const string &event);

    bool Evaluate(const Date &date, const std::string &event) const override;

private:
    Comparison comparison;
    string event;
};
