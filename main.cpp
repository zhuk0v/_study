#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <set>

#include <boost/algorithm/string/split.hpp>
#include <fmt/format.h>

#include "sqlite3.h"

#define UNUSE(x) (void)(x)

class Command {
public:
    enum class Type {
        UNKNOWN,
        INSERT_RECORD,
        TRUNCATE_TABLE,
        INTERSECTION,
        SYMMETRIC_DIFFERENCE,
    };
private:
    std::vector<std::string> m_cmd;

public:
    Command(std::string str) {
        boost::split(m_cmd, str, isspace);
    }
    ~Command() {}

    Type GetType() const {
        if (m_cmd[0] == "INSERT") {
            return Type::INSERT_RECORD;
        }
        else if (m_cmd[0] == "TRUNCATE") {
            return Type::TRUNCATE_TABLE;
        }
        else if (m_cmd[0] == "INTERSECTION") {
            return Type::INTERSECTION;
        }
        else if (m_cmd[0] == "SYMMETRIC_DIFFERENCE") {
            return Type::SYMMETRIC_DIFFERENCE;
        }
        return Type::UNKNOWN;
    }

    std::string operator[](int i) const {
        return m_cmd[i];
    }
};


class Handler {
public:
    enum class Status {
        UNKNOWN,
        OK,
        END,
    };

private:

    std::set<std::string> m_table_names;
    sqlite3* m_db = nullptr;

public:
    Handler() {
        int status = sqlite3_open("data.db", &m_db);

        // Check
        if (status != SQLITE_OK) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;
            sqlite3_close(m_db);
            return;
        }
    }

    ~Handler() {
        if (m_db) {
            sqlite3_close(m_db);
        }
    }

    Status Execute(const Command cmd) {
        switch (cmd.GetType()) {
        case Command::Type::INSERT_RECORD:
            CreateTable(cmd[1]);
            InsertRecord(cmd[1], cmd[2], cmd[3]);
            return Status::OK;
        case Command::Type::INTERSECTION:
            Intersection();
            return Status::OK;
        case Command::Type::SYMMETRIC_DIFFERENCE:
            Difference();
            return Status::OK;
        default:
            break;
        }
        return Status::UNKNOWN;
    }

private:

    void CreateTable(const std::string table_name) {
        // SQL statement
        std::string sql_st = fmt::format("CREATE TABLE IF NOT EXISTS {} ", table_name);
        sql_st += std::string("("
            "ID     INT     PRIMARY KEY     NOT NULL, "
            "NAME   TEXT                    NOT NULL"
            ");");

        // Debug
        // fmt::print("{}\n", sql_st);

        // Send SQL statement
        SendCommandToDatabase(sql_st);

        // Save table name
        m_table_names.insert(table_name);
    }

    void InsertRecord(const std::string table_name, const std::string id, const std::string name) {

        // SQL statement
        std::string sql_st = fmt::format("INSERT INTO {} ('ID', 'NAME') VALUES ('{}', '{}');", table_name, id, name);

        // Debug
        // fmt::print("{}\n", sql_st);

        // Send SQL statement
        auto status = SendCommandToDatabase(sql_st);

        if (status == SQLITE_OK) {
            fmt::print("OK\n");
        }
    }

    void Intersection() {
        int status = SQLITE_OK;
        std::vector<std::string> intersection_id{};
        // Get the intersection ID
        {
            // Callback function
            auto sql_callback = [](void* intersection_id, int num_col, char** val_col, char** name_col) -> int {

                for (int i = 0; i < num_col; i++) {
                    if (val_col[i]) {
                        ((std::vector<std::string>*)intersection_id)->push_back(val_col[i]);

                        // Debug
                        // fmt::print("INTERSECT {} = {}\n", name_col[i], val_col[i]);
                    }
                }

                return SQLITE_OK;
            };

            // SQL statement
            std::string sql_st{};

            for (auto it = m_table_names.begin(); it != m_table_names.end(); it++) {
                if (it == m_table_names.begin()) {
                    sql_st = fmt::format("SELECT ID FROM {} ", *it);
                }
                else {
                    sql_st += fmt::format("INTERSECT SELECT ID FROM {} ", *it);
                }
            }
            sql_st += ";";

            // Debug
            // fmt::print("{}\n", sql_st);


            // Send SQL statement
            status |= SendCommandToDatabase(sql_st, sql_callback, &intersection_id);
        }
        // Get Name from table and print
        {
            // Callback function
            auto sql_callback = [](void*, int num_col, char** val_col, char** name_col) -> int {

                for (int i = 0; i < num_col; i++) {
                    if (val_col[i]) {
                        fmt::print("{},", val_col[i]);
                    }
                }

                return SQLITE_OK;
            };


            for (auto& id : intersection_id) {
                fmt::print("{},", id);
                for (auto it = m_table_names.begin(); it != m_table_names.end(); it++) {
                    // SQL statement
                    std::string sql_st = fmt::format("SELECT NAME FROM {} WHERE ID={};", *it, id);
                    // Send SQL statement
                    status |= SendCommandToDatabase(sql_st, sql_callback);
                }
                fmt::print("\n");
            }
        }

        if (status == SQLITE_OK) {
            fmt::print("OK\n");
        }
    }

    void Difference() {
        int status = SQLITE_OK;

        std::size_t num_table = 0;
        for (auto it_proc_tab = m_table_names.begin(); it_proc_tab != m_table_names.end(); it_proc_tab++, num_table++) {
            std::vector<std::string> except_id{};
            // Get the except ID for table with "*it_proc_tab" name
            {
                // Callback function
                auto sql_callback = [](void* except_id, int num_col, char** val_col, char** name_col) -> int {

                    for (int i = 0; i < num_col; i++) {
                        if (val_col[i]) {
                            ((std::vector<std::string>*)except_id)->push_back(val_col[i]);

                            // Debug
                            // fmt::print("EXCEPT {} = {}\n", name_col[i], val_col[i]);
                        }
                    }

                    return SQLITE_OK;
                };

                // SQL statement
                std::string sql_st = fmt::format("SELECT ID FROM {} ", *it_proc_tab);

                for (auto it = m_table_names.begin(); it != m_table_names.end(); it++) {
                    if (*it != *it_proc_tab) {
                        sql_st += fmt::format("EXCEPT SELECT ID FROM {} ", *it);
                    }
                }
                sql_st += ";";


                // Debug
                // fmt::print("{}\n", sql_st);


                // Send SQL statement
                status |= SendCommandToDatabase(sql_st, sql_callback, &except_id);
            }

            // Get Name from table and print
            {
                // Callback function
                auto sql_callback = [](void*, int num_col, char** val_col, char** name_col) -> int {

                    for (int i = 0; i < num_col; i++) {
                        if (val_col[i]) {
                            fmt::print("{},", val_col[i]);
                        }
                    }

                    return SQLITE_OK;
                };

                for (auto& id : except_id) {
                    fmt::print("{},", id);

                    for (std::size_t i = 0; i < num_table; i++) {
                        fmt::print(",");
                    }

                    for (auto it = m_table_names.begin(); it != m_table_names.end(); it++) {
                        // SQL statement
                        std::string sql_st = fmt::format("SELECT NAME FROM {} WHERE ID={};", *it, id);
                        // Send SQL statement
                        status |= SendCommandToDatabase(sql_st, sql_callback);
                    }
                    fmt::print("\n");
                }
            }
        }

        if (status == SQLITE_OK) {
            fmt::print("OK\n");
        }
    }

    void ClearTable(const std::string table_name) {
        // SQL statement
        std::string sql_st = fmt::format("DELETE FROM '{}';", table_name);

        // Debug
        // fmt::print("{}\n", sql_st);

        // Send SQL statement
        SendCommandToDatabase(sql_st);
    }

    int SendCommandToDatabase(const std::string sql_statement, int (*callback)(void*, int, char**, char**) = NULL, void* first_callback_arg = NULL) {
        // Check
        if (!m_db) {
            return SQLITE_CANTOPEN;
        }

        char* messageError{};

        // Send
        int status = sqlite3_exec(m_db, sql_statement.c_str(), callback, first_callback_arg, &messageError);

        // Check
        if (status != SQLITE_OK) {
            fmt::print("Error: {}\n", messageError);
            sqlite3_free(messageError);
        }

        return status;
    }

    // bool CheckExistId(const std::string table_name, const std::string id) {
    //     // Find parameter
    //     std::size_t num_rec_with_curr_id{ 0 };
    //     // Callback function
    //     auto sql_callback = [](void* ptr_num_rec_with_curr_id, int num_row, char**, char**) -> int {
    //         if (!ptr_num_rec_with_curr_id) {
    //             return SQLITE_ABORT;
    //         }
    //
    //         *((std::size_t*)ptr_num_rec_with_curr_id) = static_cast<std::size_t>(num_row);
    //
    //         return SQLITE_OK;
    //     };
    //
    //     // SQL statement
    //     std::string sql_st = fmt::format("SELECT EXISTS (SELECT * FROM '{}' where 'ID' = {});", table_name, id);
    //
    //     // Debug
    //     // fmt::print("{}\n", sql_st);
    //
    //     // Send SQL statement
    //     SendCommandToDatabase(sql_st, sql_callback, &num_rec_with_curr_id);
    //
    //     return static_cast<bool>(num_rec_with_curr_id);
    // }
};




int main(int, char const**) {
    // std::system("rm /home/sergey/otus/_study/data.db");

    std::string cmd_str{};
    Handler handler{};

    // std::vector<std::string> test_cmd_sequence = {
    //     "INSERT A 0 lean",
    //     "INSERT A 0 understand",
    //     "INSERT A 1 sweater",
    //     "INSERT A 2 frank",
    //     "INSERT A 3 violation",
    //     "INSERT A 4 quality",
    //     "INSERT A 5 precision",
    //
    //     "INSERT B 3 proposal",
    //     "INSERT B 4 example",
    //     "INSERT B 5 lake",
    //     "INSERT B 6 flour",
    //     "INSERT B 7 wonder",
    //     "INSERT B 8 selection",
    // };
    // for (auto& cmd : test_cmd_sequence) {
    //     auto cmd_obj = Command(cmd);
    //     if (handler.Execute(cmd_obj) == Handler::Status::END) {
    //         break;
    //     }
    // }

    for (;;) {
        std::getline(std::cin, cmd_str);
        auto cmd_obj = Command(cmd_str);
        if (handler.Execute(cmd_obj) == Handler::Status::END) {
            break;
        }
    }
    return EXIT_SUCCESS;
}
