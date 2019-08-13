#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <tuple>
#include <sstream>
#include <vector>
#include "date.h"

Date::Date(int new_year, int new_month, int new_day) : year(new_year), month(new_month), day(new_day) {
    if (month > 12 || month < 1) {
        throw std::logic_error(
                "Month value is invalid: " + std::to_string(month));
    }

    if (day > 31 || day < 1) {
        throw std::logic_error(
                "Day value is invalid: " + std::to_string(day));
    }
}

Date::~Date() {}

int Date::GetYear() const {
    return year;
}

int Date::GetMonth() const {
    return month;
}

int Date::GetDay() const {
    return day;
}

std::string Date::ToString() const {
    return std::to_string(year) + "-"
           + std::to_string(month) + "-"
           + std::to_string(day);
}

bool operator<(const Date &lhs, const Date &rhs) {
    return std::make_tuple(lhs.GetYear(), lhs.GetMonth(), lhs.GetDay())
           < std::make_tuple(rhs.GetYear(), rhs.GetMonth(), rhs.GetDay());
}

bool operator==(const Date &lhs, const Date &rhs) {
    return std::make_tuple(lhs.GetYear(), lhs.GetMonth(), lhs.GetDay())
           == std::make_tuple(rhs.GetYear(), rhs.GetMonth(), rhs.GetDay());
}

bool operator!=(const Date &lhs, const Date &rhs) {
    return std::make_tuple(lhs.GetYear(), lhs.GetMonth(), lhs.GetDay())
           != std::make_tuple(rhs.GetYear(), rhs.GetMonth(), rhs.GetDay());
}

bool operator<=(const Date &lhs, const Date &rhs) {
    return std::make_tuple(lhs.GetYear(), lhs.GetMonth(), lhs.GetDay())
           <= std::make_tuple(rhs.GetYear(), rhs.GetMonth(), rhs.GetDay());
}

bool operator>(const Date &lhs, const Date &rhs) {
    return std::make_tuple(lhs.GetYear(), lhs.GetMonth(), lhs.GetDay())
           > std::make_tuple(rhs.GetYear(), rhs.GetMonth(), rhs.GetDay());
}

bool operator>=(const Date &lhs, const Date &rhs) {
    return std::make_tuple(lhs.GetYear(), lhs.GetMonth(), lhs.GetDay())
           >= std::make_tuple(rhs.GetYear(), rhs.GetMonth(), rhs.GetDay());
}

std::ostream &operator<<(std::ostream &stream, const Date &date) {
    stream << std::setw(4) << std::setfill('0') << date.GetYear() << "-"
           << std::setw(2) << std::setfill('0') << date.GetMonth() << "-"
           << std::setw(2) << std::setfill('0') << date.GetDay();
    return stream;
}

std::ostream &operator<<(std::ostream &stream,
                         const std::pair<Date, std::set<std::string>> &events) {
    for (auto &event : events.second) {
        stream << events.first << " " << event;
    }

    return stream;
}

Date ParseDate(std::istream &date_stream) {
    bool ok = true;

    int year;
    ok = ok && (date_stream >> year);
    ok = ok && (date_stream.peek() == '-');
    date_stream.ignore(1);

    int month;
    ok = ok && (date_stream >> month);
    ok = ok && (date_stream.peek() == '-');
    date_stream.ignore(1);

    int day;
    ok = ok && (date_stream >> day);

    if (!ok) {
        throw std::logic_error("Wrong date format");
    }
    return Date(year, month, day);
}
