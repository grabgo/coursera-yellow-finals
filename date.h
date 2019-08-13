#pragma once

#include <iostream>
#include <set>
#include <string>
#include <tuple>

class Date {
public:
    Date(int new_year, int new_month, int new_day);

    ~Date();

    int GetYear() const;

    int GetMonth() const;

    int GetDay() const;

    std::string ToString() const;

private:
    int year;
    int month;
    int day;
};

bool operator<(const Date &lhs, const Date &rhs);

bool operator==(const Date &lhs, const Date &rhs);

bool operator!=(const Date &lhs, const Date &rhs);

bool operator<=(const Date &lhs, const Date &rhs);

bool operator>=(const Date &lhs, const Date &rhs);

bool operator>(const Date &lhs, const Date &rhs);

std::ostream &operator<<(std::ostream &stream, const Date &date);

std::ostream &operator<<(std::ostream &stream,
                         const std::pair<Date, std::set<std::string>> &events);

Date ParseDate(std::istream &date_stream);
