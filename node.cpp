#include "node.h"

LogicalOperationNode::LogicalOperationNode(LogicalOperation operation,
                                           shared_ptr<Node> left, shared_ptr<Node> right) :
        left(left), right(right), operation(operation) {
}

bool LogicalOperationNode::Evaluate(const Date &date,
                                    const std::string &event) const {
    if (operation == LogicalOperation::And) {
        return left.get()->Evaluate(date, event)
               && right.get()->Evaluate(date, event);
    } else {
        return left.get()->Evaluate(date, event)
               || right.get()->Evaluate(date, event);
    }
}

bool EmptyNode::Evaluate(const Date &date, const std::string &event) const {
    return true;
}

DateComparisonNode::DateComparisonNode(const Comparison &comparison,
                                       const Date &date) :
        comparison(comparison), date(date) {
}

bool DateComparisonNode::Evaluate(const Date &date, const std::string &event) const {
    switch (comparison) {
        case Comparison::Equal:
            return date == this->date;
        case Comparison::Greater:
            return date > this->date;
        case Comparison::GreaterOrEqual:
            return date >= this->date;
        case Comparison::Less:
            return date < this->date;
        case Comparison::LessOrEqual:
            return date <= this->date;
        case Comparison::NotEqual:
            return date != this->date;
    }
    return false;
}

EventComparisonNode::EventComparisonNode(const Comparison &comparison,
                                         const string &event) :
        comparison(comparison), event(event) {
}

bool EventComparisonNode::Evaluate(const Date &date, const std::string &event) const {

    if (event == "{%signal%pill%}") return true;

    switch (comparison) {
        case Comparison::Equal:
            return event == this->event;
        case Comparison::Greater:
            return event > this->event;
        case Comparison::GreaterOrEqual:
            return event >= this->event;
        case Comparison::Less:
            return event < this->event;
        case Comparison::LessOrEqual:
            return event <= this->event;
        case Comparison::NotEqual:
            return event != this->event;
    }
    return false;
}
