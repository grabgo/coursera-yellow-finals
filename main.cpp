#include "database.h"
#include "date.h"
#include "condition_parser.h"
#include "node.h"
#include "test_runner.h"

#include <iostream>
#include <stdexcept>

using namespace std;

string ParseEvent(istream &is) {
    string event;

    while (is.peek() == ' ')
        is.ignore(1);

    getline(is, event);
    return event;
}

void TestAll();

int main() {
    TestAll();

    Database db;

    for (string line; getline(cin, line);) {
        istringstream is(line);

        string command;
        is >> command;
        if (command == "Add") {
            const Date date = ParseDate(is);
            const string event = ParseEvent(is);
            db.Add(date, event);
        } else if (command == "Print") {
            db.Print(cout);
        } else if (command == "Del") {
            auto condition = ParseCondition(is);
            auto predicate =
                    [condition](const Date &date, const string &event) {
                        return condition->Evaluate(date, event);
                    };
            int count = db.RemoveIf(predicate);
            cout << "Removed " << count << " entries" << endl;
        } else if (command == "Find") {
            auto condition = ParseCondition(is);
            auto predicate =
                    [condition](const Date &date, const string &event) {
                        return condition->Evaluate(date, event);
                    };

            const auto entries = db.FindIf(predicate);
            for (const auto &entry : entries) {
                cout << entry.first << " " << entry.second << endl;
            }
            cout << "Found " << entries.size() << " entries" << endl;
        } else if (command == "Last") {
            try {
                cout << db.Last(ParseDate(is)) << endl;
            } catch (invalid_argument &) {
                cout << "No entries" << endl;
            }
        } else if (command.empty()) {
            continue;
        } else {
            throw logic_error("Unknown command: " + command);
        }
    }

    return 0;
}

void TestParseEvent() {
    {
        istringstream is("event");
        AssertEqual(ParseEvent(is), "event",
                    "Parse event without leading spaces");
    }
    {
        istringstream is("   sport event ");
        AssertEqual(ParseEvent(is), "sport event ",
                    "Parse event with leading spaces");
    }
    {
        istringstream is("  first event  \n  second event");
        vector<string> events;
        events.push_back(ParseEvent(is));
        events.push_back(ParseEvent(is));
        AssertEqual(events, vector<string>{"first event  ", "second event"},
                    "Parse multiple events");
    }
}

void TestRemoveIf() {
    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        std::stringstream stream("date > 1992-12-1");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        int deleted = db.RemoveIf(predicate);
        AssertEqual(deleted, 2u, "Remove if works incorrectly #1#1");
        AssertEqual(db.GetHistoryEventSize(), 2u, "Remove if works incorrectly #1#2");
        AssertEqual(db.GetStorageEventSize(), 2u, "Remove if works incorrectly #1#3");
        AssertEqual(db.GetHistorySize(), 1u, "Remove if works incorrectly #1#4");
        AssertEqual(db.GetStorageSize(), 1u, "Remove if works incorrectly #1#5");
    }

    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        std::stringstream stream("date == 1992-12-1 OR date > 1992-12-1");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        int deleted = db.RemoveIf(predicate);
        AssertEqual(deleted, 4u, "Remove if works incorrectly #2#1");
        AssertEqual(db.GetHistoryEventSize(), 0u, "Remove if works incorrectly #2#2");
        AssertEqual(db.GetStorageEventSize(), 0u, "Remove if works incorrectly #2#3");
        AssertEqual(db.GetHistorySize(), 0u, "Remove if works incorrectly #2#4");
        AssertEqual(db.GetStorageSize(), 0u, "Remove if works incorrectly #2#5");
    }

    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        std::stringstream stream("");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        int deleted = db.RemoveIf(predicate);
        AssertEqual(deleted, 4u, "Remove if works incorrectly #3#1");
        AssertEqual(db.GetHistoryEventSize(), 0u, "Remove if works incorrectly #3#2");
        AssertEqual(db.GetStorageEventSize(), 0u, "Remove if works incorrectly #3#3");
        AssertEqual(db.GetHistorySize(), 0u, "Remove if works incorrectly #3#4");
        AssertEqual(db.GetStorageSize(), 0u, "Remove if works incorrectly #3#5");
    }

    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        std::stringstream stream("date == 1992-12-1");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        int deleted = db.RemoveIf(predicate);
        AssertEqual(deleted, 1u, "Remove if works incorrectly #4#1");
        AssertEqual(db.GetHistoryEventSize(), 2u, "Remove if works incorrectly #4#2");
        AssertEqual(db.GetStorageEventSize(), 2u, "Remove if works incorrectly #4#3");
        AssertEqual(db.GetHistorySize(), 2u, "Remove if works incorrectly #4#4");
        AssertEqual(db.GetStorageSize(), 2u, "Remove if works incorrectly #4#5");
    }

    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "baseball");
        db.Add(Date(1992, 12, 1), "handball");

        std::stringstream stream(R"(event == "tennis")");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        int deleted = db.RemoveIf(predicate);
        AssertEqual(deleted, 1u, "Remove if works incorrectly #5#1");
        AssertEqual(db.GetHistoryEventSize(), 2u, "Remove if works incorrectly #5#2");
        AssertEqual(db.GetStorageEventSize(), 2u, "Remove if works incorrectly #5#3");
        AssertEqual(db.GetHistorySize(), 1u, "Remove if works incorrectly #5#4");
        AssertEqual(db.GetStorageSize(), 1u, "Remove if works incorrectly #5#5");
    }

    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "baseball");
        db.Add(Date(1992, 12, 1), "handball");

        std::stringstream stream(R"(event == "")");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        int deleted = db.RemoveIf(predicate);
        AssertEqual(deleted, 0u, "Remove if works incorrectly #6#1");
        AssertEqual(db.GetHistoryEventSize(), 3u, "Remove if works incorrectly #6#2");
        AssertEqual(db.GetStorageEventSize(), 3u, "Remove if works incorrectly #6#3");
        AssertEqual(db.GetHistorySize(), 1u, "Remove if works incorrectly #6#4");
        AssertEqual(db.GetStorageSize(), 1u, "Remove if works incorrectly #6#5");
    }

    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis1");
        db.Add(Date(1992, 12, 1), "tennis2");
        db.Add(Date(1992, 12, 1), "baseball1");
        db.Add(Date(1992, 12, 1), "handball1");
        db.Add(Date(1992, 12, 1), "handball2");
        db.Add(Date(1992, 12, 1), "handball3");
        db.Add(Date(1992, 12, 1), "handball4");

        std::stringstream stream(R"(event == "handball1")");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        int deleted = db.RemoveIf(predicate);
        AssertEqual(deleted, 1u, "Remove if works incorrectly #7#1");
        AssertEqual(db.GetHistoryEventSize(), 6u, "Remove if works incorrectly #7#2");
        AssertEqual(db.GetStorageEventSize(), 6u, "Remove if works incorrectly #7#3");
        AssertEqual(db.GetHistorySize(), 1u, "Remove if works incorrectly #7#4");
        AssertEqual(db.GetStorageSize(), 1u, "Remove if works incorrectly #7#5");
    }
}

void TestFindIf() {
    {
        // Basic date comparison
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        std::stringstream stream("date > 1992-12-1");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        auto result = db.FindIf(predicate);
        AssertEqual(result.size(), 2u, "Find if works incorrectly #1");
    }

    {
        // And date comparison
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        std::stringstream stream("date >= 1992-12-1 AND date < 1992-12-10");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        auto result = db.FindIf(predicate);
        AssertEqual(result.size(), 3u, "Find if works incorrectly #2");
    }

    {
        // And date and event comparison
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        std::stringstream stream("date == 1992-12-1 AND event == \"football\"");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        auto result = db.FindIf(predicate);
        AssertEqual(result.size(), 1u, "Find if works incorrectly #3");
    }

    {
        // Event equals comparison
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        std::stringstream stream("event == \"football\"");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        auto result = db.FindIf(predicate);
        AssertEqual(result.size(), 1u, "Find if works incorrectly #4");
    }

    {
        // Event OR comparison
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        std::stringstream stream(R"(event == "football" OR event == "handball")");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        auto result = db.FindIf(predicate);
        AssertEqual(result.size(), 2u, "Find if works incorrectly #5");
    }

    {
        // Complex OR comparison
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        std::stringstream stream(R"(event == "football" OR (event == "handball" AND date > 1993-1-1))");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        auto result = db.FindIf(predicate);
        AssertEqual(result.size(), 1u, "Find if works incorrectly #6");
    }

    {
        // Complex OR comparison
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        std::stringstream stream("");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        auto result = db.FindIf(predicate);
        AssertEqual(result.size(), 4u, "Find if works incorrectly #7");
    }
}

void TestLast() {
    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        auto result = db.Last(Date(1992, 12, 10));
        AssertEqual(result, "1992-12-10 handball", "Last works incorrectly #1");
    }

    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        auto result = db.Last(Date(1993, 12, 20));
        AssertEqual(result, "1992-12-10 handball", "Last works incorrectly #2");
    }

    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        auto result = db.Last(Date(1992, 12, 1));
        AssertEqual(result, "1992-12-01 football", "Last works incorrectly #3");
    }

    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        auto result = db.Last(Date(1991, 12, 1));
        AssertEqual(result, "No entries", "Last works incorrectly #4");
    }

    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        std::stringstream stream(R"(event == "handball")");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        db.RemoveIf(predicate);
        auto result = db.Last(Date(1993, 12, 1));
        AssertEqual(result, "1992-12-02 baseball", "Last works incorrectly #5");
    }

    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");
        db.Add(Date(1992, 12, 1), "videogames");

        auto result = db.Last(Date(1992, 12, 1));
        AssertEqual(result, "1992-12-01 videogames", "Last works incorrectly #6");
    }

    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");
        db.Add(Date(1992, 12, 1), "football");
        db.Add(Date(1992, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        auto result = db.Last(Date(1992, 12, 2));
        AssertEqual(result, "1992-12-02 baseball", "Last works incorrectly #7#1");

        std::stringstream stream(R"(event == "baseball")");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        db.RemoveIf(predicate);
        result = db.Last(Date(1992,
12, 2));
        AssertEqual(result, "1992-12-01 football", "Last works incorrectly #7#2");
    }

    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis");

        auto result = db.Last(Date(1992, 12, 2));
        AssertEqual(result, "1992-12-01 tennis", "Last works incorrectly #8#1");

        result = db.Last(Date(1991, 12, 2));
        AssertEqual(result, "No entries", "Last works incorrectly #8#2");

        result = db.Last(Date(1995, 12, 2));
        AssertEqual(result, "1992-12-01 tennis", "Last works incorrectly #8#3");
    }

    {
        Database db;

        db.Add(Date(1998, 12, 1), "tennis");
        db.Add(Date(1997, 12, 1), "football");
        db.Add(Date(1995, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        auto result = db.Last(Date(1999, 12, 2));
        AssertEqual(result, "1998-12-01 tennis", "Last works incorrectly #9");
    }

    {
        Database db;

        db.Add(Date(1998, 12, 1), "tennis");
        db.Add(Date(1998, 12, 1), "netflix");
        db.Add(Date(1998, 12, 1), "chill");
        db.Add(Date(1997, 12, 1), "football");
        db.Add(Date(1995, 12, 2), "baseball");
        db.Add(Date(1992, 12, 10), "handball");

        std::stringstream stream(R"(date == "1998-12-1")");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        db.RemoveIf(predicate);
        auto result = db.Last(Date(1999, 12, 2));
        AssertEqual(result, "1997-12-01 football", "Last works incorrectly #10");
    }

    {
        Database db;

        db.Add(Date(1998, 12, 1), "tennis");

        std::stringstream stream(R"(event == "tennis")");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        db.RemoveIf(predicate);
        auto result = db.Last(Date(1998, 12, 1));
        AssertEqual(result, "No entries", "Last works incorrectly #11");
    }

    {
        Database db;

        db.Add(Date(1998, 12, 1), "tennis");
        db.Add(Date(1998, 12, 1), "ping pong");
        db.Add(Date(1998, 12, 1), "tennis");
        db.Add(Date(1998, 12, 1), "ping pong");
        db.Add(Date(1998, 12, 1), "tennis");
        db.Add(Date(1998, 12, 1), "ping pong");

        auto result = db.Last(Date(1998, 12, 1));
        AssertEqual(result, "1998-12-01 ping pong", "Last works incorrectly #12#1");

        std::stringstream stream(R"(event == "ping pong")");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        db.RemoveIf(predicate);
        result = db.Last(Date(1998, 12, 1));
        AssertEqual(result, "1998-12-01 tennis", "Last works incorrectly #12#2");
    }

    {
        Database db;

        db.Add(Date(1998, 12, 1), "tennis");
        db.Add(Date(1998, 12, 1), "ping pong");
        db.Add(Date(1998, 12, 1), "tennis");
        db.Add(Date(1998, 12, 1), "ping pong");
        db.Add(Date(1998, 12, 1), "tennis");
        db.Add(Date(1998, 12, 1), "ping pong");

        auto result = db.Last(Date(1998, 12, 1));
        AssertEqual(result, "1998-12-01 ping pong", "Last works incorrectly #12#1");

        std::stringstream stream(R"(event == "ping pong")");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        db.RemoveIf(predicate);
        result = db.Last(Date(1998, 12, 1));
        AssertEqual(result, "1998-12-01 tennis", "Last works incorrectly #12#2");
    }

    {
        Database db;

        db.Add(Date(1998, 12, 4), "ping pong");
        db.Add(Date(1998, 12, 4), "ping pong2");
        db.Add(Date(1998, 12, 4), "ping pong3");
        db.Add(Date(1998, 12, 4), "ping pong4");
        db.Add(Date(1998, 12, 5), "tennis");
        db.Add(Date(1998, 12, 5), "tennis2");
        db.Add(Date(1998, 12, 5), "tennis3");
        db.Add(Date(1998, 12, 5), "tennis4");
        db.Add(Date(1998, 12, 5), "tennis");
        db.Add(Date(1998, 12, 6), "ping pong");

        auto result = db.Last(Date(1998, 12, 5));
        AssertEqual(result, "1998-12-05 tennis4", "Last works incorrectly #13#1");

        std::stringstream stream(R"(date == 1998-12-5 OR date == 1998-12-6)");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        db.RemoveIf(predicate);
        result = db.Last(Date(1998, 12, 6));
        AssertEqual(result, "1998-12-04 ping pong4", "Last works incorrectly #13#2");
    }

    {
        Database db;

        db.Add(Date(1998, 12, 11), "event");
        db.Add(Date(1998, 12, 11), "event2");
        db.Add(Date(1998, 12, 11), "event3");

        auto result = db.Last(Date(1998, 12, 11));
        AssertEqual(result, "1998-12-11 event3", "Last works incorrectly #14#1");

        std::stringstream stream(R"(event == "event")");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        db.RemoveIf(predicate);

        result = db.Last(Date(1998, 12, 11));
        AssertEqual(result, "1998-12-11 event3", "Last works incorrectly #14#2");
    }

    {
        Database db;

        db.Add(Date(1992, 12, 1), "tennis1");
        db.Add(Date(1992, 12, 1), "tennis2");
        db.Add(Date(1992, 12, 1), "baseball1");
        db.Add(Date(1992, 12, 1), "handball1");
        db.Add(Date(1992, 12, 1), "handball2");
        db.Add(Date(1992, 12, 1), "handball3");
        db.Add(Date(1992, 12, 1), "handball4");

        std::stringstream stream(R"(event != "handball1")");
        shared_ptr<Node> condition = ParseCondition(stream);

        auto predicate = [condition](const Date &date, const string &event) {
            return condition->Evaluate(date, event);
        };

        db.RemoveIf(predicate);
        auto result = db.Last(Date(1998, 12, 1));
        AssertEqual(result, "1992-12-01 handball1", "Last works incorrectly #14");
    }
}

void TestPrint() {
    {
        Database db;

        db.Add(Date(1998, 12, 1), "tennis");
        db.Add(Date(1998, 12, 1), "netflix");
        db.Add(Date(1998, 12, 1), "chill");

        stringstream stream;

        db.Print(stream);

        AssertEqual(
                stream.str(),
                "1998-12-01 tennis\n1998-12-01 netflix\n1998-12-01 chill\n",
                "Print works incorrectly #1"
        );
    }

    {
        Database db;

        db.Add(Date(1998, 12, 1), "tennis");
        db.Add(Date(1998, 12, 1), "tennis");
        db.Add(Date(1998, 12, 1), "tennis");
        db.Add(Date(1998, 12, 1), "netflix");
        db.Add(Date(1998, 12, 1), "netflix");
        db.Add(Date(1998, 12, 1), "chill");
        db.Add(Date(1998, 12, 1), "chill");
        db.Add(Date(1998, 12, 1), "chill");
        db.Add(Date(1998, 12, 1), "chill");
        db.Add(Date(1998, 12, 1), "chill");

        stringstream stream;

        db.Print(stream);

        AssertEqual(
                stream.str(),
                "1998-12-01 tennis\n1998-12-01 netflix\n1998-12-01 chill\n",
                "Print works incorrectly #2"
        );
    }

    {
        Database db;

        db.Add(Date(1998, 12, 1), "tennis");
        db.Add(Date(1998, 12, 1), "netflix");
        db.Add(Date(1998, 12, 1), "tennis");
        db.Add(Date(1998, 12, 1), "chill");

        stringstream stream;

        db.Print(stream);

        AssertEqual(
                stream.str(),
                "1998-12-01 tennis\n1998-12-01 netflix\n1998-12-01 chill\n",
                "Print works incorrectly #3"
        );
    }
}

void TestAll() {
    TestRunner tr;
    tr.RunTest(TestParseEvent, "TestParseEvent");
    tr.RunTest(TestFindIf, "TestFindIf");
    tr.RunTest(TestRemoveIf, "TestRemoveIf");
    tr.RunTest(TestLast, "TestLast");
    tr.RunTest(TestPrint, "TestPrint");
    //tr.RunTest(TestParseCondition, "TestParseCondition");
}
