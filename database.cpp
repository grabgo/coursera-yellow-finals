#include <iostream>
#include <sstream>
#include "database.h"

void Database::Add(const Date &date, const std::string &event) {
    if (event.empty())
        return;

    auto it = storage[date].insert(event);

    if (it.second) {
        history[date].push_back(event);
    }
}

void Database::Print(std::ostream &os) const {
    for (const auto &item : history) {

        if (item.second.empty()) continue;

        for (const std::string &event : item.second) {
            os << item.first << " " << event << std::endl;
        }
    }
}

std::string Database::Last(const Date &date) const {
    auto upperBound = history.upper_bound(date);

    if (upperBound == history.begin()) {
        return "No entries";
    }

    auto result = std::prev(upperBound);

    std::stringstream os;

    os << result->first << " " << result-> second.back();
    return os.str();
}

int Database::GetHistoryEventSize() const {
    int count = 0;
    for (auto &item : history) {
        count += item.second.size();
    }

    return count;
}

int Database::GetStorageEventSize() const {
    int count = 0;
    for (auto &item : storage) {
        count += item.second.size();
    }

    return count;
}

int Database::GetHistorySize() const {
    return history.size();
}

int Database::GetStorageSize() const {
    return storage.size();
}
