#pragma once

#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include "date.h"

class Database {
public:
    void Add(const Date &date, const std::string &event);

    void Print(std::ostream &os) const;

    std::string Last(const Date &date) const;

    template<typename Predicate>
    std::vector<std::pair<Date, std::string>> FindIf(Predicate predicate) const {
        std::vector<std::pair<Date, std::string>> result;

        for (const auto &item : history) {

            const Date &date = item.first;

            /** Check whether current date is applicable to current predicate. This string defined in EventComparisonNode
             and always returns true for any comparison, which allows to avoid unnecessary comparisons.
             **/
            if (!predicate(date, "{%signal%pill%}")) continue;

            // Here we iterate through history to maintain order in which elements were added
            for (const std::string &event : history.at(date)) {
                if (predicate(date, event))
                    result.emplace_back(date, event);
            }
        }

        return result;
    };

    template<typename Predicate>
    int RemoveIf(Predicate predicate) {
        int deleted = 0;

        // Iterating through map<Date, set<string>> - unique storage
        for (auto storage_iter = storage.begin(); storage_iter != storage.end();) {

            /** Check whether current date is applicable to current predicate. This string defined in EventComparisonNode
             and always returns true for any comparison, which allows to avoid unnecessary comparisons.
             **/
            const Date &date = storage_iter->first;
            if (!predicate(date, "{%signal%pill%}")) {
                storage_iter++;
                continue;
            }

            std::set<std::string> &storage_events = storage_iter->second;
            std::vector<std::string> &history_events = history.at(date);

            for (auto storage_events_iter = storage_events.begin(); storage_events_iter != storage_events.end();) {

                if (!predicate(date, *storage_events_iter)) {
                    storage_events_iter++;
                    continue;
                }

                storage_events_iter = storage_events.erase(storage_events_iter);
                deleted++;
            }

            // remove if for vector which stores history of operations
            auto history_border = std::stable_partition(
                    history_events.begin(),
                    history_events.end(),
                    [predicate, date](const std::string &event) {
                        return !predicate(date, event);
                    }
            );

            history_events.erase(history_border, history_events.end());

            //if all elements within vector have been deleted we clear map record
            if (history_events.empty()) {
                history.erase(date);
            }

            // same goes for set
            if (storage_events.empty()) {
                storage_iter = storage.erase(storage_iter);
                continue;
            }

            storage_iter++;
        }

        return deleted;
    };

    int GetHistoryEventSize() const;

    int GetHistorySize() const;

    int GetStorageEventSize() const;

    int GetStorageSize() const;

private:
    std::map<Date, std::set<std::string>> storage;
    std::map<Date, std::vector<std::string>> history;
};
