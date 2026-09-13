#include "src/compiler.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

// --- CROSS-PLATFORM HEADERS ---
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace {

constexpr const char *VERSION = "1.0.0";

class RawTerminal {
public:
    RawTerminal() = default;

    bool enable() {
        if (active_) return true;
#ifdef _WIN32
        hStdin_ = GetStdHandle(STD_INPUT_HANDLE);
        if (hStdin_ == INVALID_HANDLE_VALUE || !GetConsoleMode(hStdin_, &original_)) return false;
        
        // Disable line buffering and echo on Windows
        DWORD raw = original_ & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
        if (!SetConsoleMode(hStdin_, raw)) return false;
#else
        if (tcgetattr(STDIN_FILENO, &original_) == -1) return false;
        termios raw = original_;
        raw.c_iflag &= static_cast<tcflag_t>(~(ICRNL | IXON));
        raw.c_lflag &= static_cast<tcflag_t>(~(ECHO | ICANON));
        raw.c_oflag &= static_cast<tcflag_t>(~OPOST);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        if (tcsetattr(STDIN_FILENO, TCSADRAIN, &raw) == -1) return false;
#endif
        active_ = true;
        return true;
    }

    void restore() {
        if (active_) {
            std::cout << std::flush;
#ifdef _WIN32
            SetConsoleMode(hStdin_, original_);
#else
            tcsetattr(STDIN_FILENO, TCSADRAIN, &original_);
#endif
            active_ = false;
        }
    }

    ~RawTerminal() { restore(); }

private:
#ifdef _WIN32
    HANDLE hStdin_;
    DWORD original_{0};
#else
    termios original_{};
#endif
    bool active_{false};
};

bool checkIsTerminal() {
#ifdef _WIN32
    return _isatty(_fileno(stdin)) && _isatty(_fileno(stdout));
#else
    return isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);
#endif
}

bool readSingleChar(char &ch) {
#ifdef _WIN32
    int res = _getch(); // Windows native unbuffered character read
    if (res == EOF) return false;
    ch = static_cast<char>(res);
    return true;
#else
    return read(STDIN_FILENO, &ch, 1) == 1; // POSIX read
#endif
}

std::string mergeLines(const std::string &query) {
    std::string merged;
    merged.reserve(query.size());
    for (char ch : query) {
        if (ch == '\r') {
            continue;
        }
        if (ch == '\n') {
            if (!merged.empty() && merged.back() != ' ') {
                merged.push_back(' ');
            }
            continue;
        }
        merged.push_back(ch);
    }
    return merged;
}

void printCommands() {
    std::cout
        << "1.  SHOW DATABASES\n"
        << "2.  CREATE DATABASE \"db_name\"\n"
        << "3.  LINK DATABASE \"db_name\"\n"
        << "4.  UNLINK DATABASE\n"
        << "5.  DELETE DATABASE \"db_name\"\n"
        << "6.  SHOW TABLES\n"
        << "7.  SHOW TABLE SCHEMA \"table_name\"\n"
        << "8.  DELETE TABLE \"table_name\"\n"
        << "9.  CREATE TABLE \"table_name\" (...)\n"
        << "10. INSERT INTO \"table_name\" VALUES (...)\n"
        << "11. SELECT ... FROM \"table_name\" [WHERE ...] [ORDERBY ...] [LIMIT n]\n"
        << "12. DELETE FROM \"table_name\" [WHERE ...]\n"
        << "13. UPDATE \"table_name\" SET col = value [, ...] [WHERE ...]\n";
}

void printCommandHelp(const std::string &command) {
    if (command == "1") {
        std::cout << "SHOW DATABASES;\nLists databases in the active directory.\n";
    } else if (command == "2") {
        std::cout << "CREATE DATABASE \"db_name\";\nCreates db_name.db in the active directory.\n";
    } else if (command == "3") {
        std::cout << "LINK DATABASE \"db_name\";\nOpens an existing database for table and row commands.\n";
    } else if (command == "4") {
        std::cout << "UNLINK DATABASE;\nCloses the currently linked database.\n";
    } else if (command == "5") {
        std::cout << "DELETE DATABASE \"db_name\";\nDeletes db_name.db from the active directory.\n";
    } else if (command == "6") {
        std::cout << "SHOW TABLES;\nLists tables in the linked database.\n";
    } else if (command == "7") {
        std::cout << "SHOW TABLE SCHEMA \"table_name\";\nShows names, types, and key information for a table.\n";
    } else if (command == "8") {
        std::cout << "DELETE TABLE \"table_name\";\nDeletes a table from the linked database.\n";
    } else if (command == "9") {
        std::cout
            << "CREATE TABLE \"table_name\" (col1 TYPE SIZE, col2 TYPE SIZE, PRIMARY KEY (col1));\n"
            << "TYPE is INT or VARCHAR. SIZE is the column storage size.\n";
    } else if (command == "10") {
        std::cout
            << "INSERT INTO \"table_name\" VALUES (value1, value2), (value1, value2);\n"
            << "Each value group must match the table's column order and types.\n";
    } else if (command == "11") {
        std::cout
            << "SELECT ALL | col1 [, col2 ...] FROM \"table_name\" [WHERE expression] "
               "[ORDERBY output_col [ASC|DESC] [, ...]] [LIMIT positive_integer];\n"
            << "WHERE, ORDERBY, and LIMIT are optional and must appear in that order.\n";
    } else if (command == "12") {
        std::cout
            << "DELETE FROM \"table_name\" [WHERE expression];\n"
            << "Without WHERE, every row in the table is deleted.\n";
    } else if (command == "13") {
        std::cout
            << "UPDATE \"table_name\" SET col1 = value1 [, col2 = value2 ...] [WHERE expression];\n"
            << "Primary-key columns cannot be updated. Without WHERE, every row is updated.\n";
    } else {
        std::cerr << "Unknown command '" << command << "'. Use a number from 1 to 13.\n";
    }
}

void printUsage() {
    std::cout
        << "Usage:\n"
        << "  bsql                     Start the interactive SQL editor in the current directory.\n"
        << "  bsql /path/to/directory  Start the editor with that database directory.\n"
        << "  bsql commands            List the 13 SQL commands.\n"
        << "  bsql command <1-13>      Explain one SQL command.\n"
        << "  bsql --version           Print the version.\n"
        << "  bsql --help              Show this help.\n\n"
        << "Editor controls:\n"
        << "  i       enter insert mode\n"
        << "  Esc     return to normal mode\n"
        << "  :e Enter execute the current multiline query\n"
        << "  :q Enter quit\n";
}

void printNormalPrompt() {
    std::cout << "\r\n-- NORMAL --  i: insert, :e: execute, :q: quit\r\n" << std::flush;
}

void runEditor(const std::string &path) {
    if (!checkIsTerminal()) {
        std::cerr << "bsql's interactive editor requires a terminal.\n";
        return;
    }

    Compiler compiler(path);
    RawTerminal terminal;
    if (!terminal.enable()) {
        std::cerr << "Unable to configure terminal: " << std::strerror(errno) << '\n';
        return;
    }

    enum class Mode { Normal, Insert, Command };
    Mode mode = Mode::Normal;
    std::string query;
    std::string command;

    std::cout << "bsql " << VERSION << "  directory: " << path;
    printNormalPrompt();

    while (true) {
        char ch = '\0';
        if (!readSingleChar(ch)) {
            break;
        }

        if (mode == Mode::Normal) {
            if (ch == 'i') {
                mode = Mode::Insert;
                std::cout << "-- INSERT --\r\n" << std::flush;
            } else if (ch == ':') {
                mode = Mode::Command;
                command.clear();
                std::cout << ':' << std::flush;
            }
            continue;
        }

        if (mode == Mode::Insert) {
            if (ch == 27) { // ESC key
                mode = Mode::Normal;
                printNormalPrompt();
            } else if (ch == 127 || ch == 8) { // Backspace
                if (!query.empty()) {
                    query.pop_back();
                    std::cout << "\b \b" << std::flush;
                }
            } else if (ch == '\r' || ch == '\n') {
                query.push_back('\n');
                std::cout << "\r\n" << std::flush;
            } else {
                query.push_back(ch);
                std::cout << ch << std::flush;
            }
            continue;
        }

        if (mode == Mode::Command) {
            if (ch == 27) { // ESC key
                mode = Mode::Normal;
                printNormalPrompt();
            } else if (ch == 127 || ch == 8) { // Backspace
                if (!command.empty()) {
                    command.pop_back();
                    std::cout << "\b \b" << std::flush;
                }
            } else if (ch == '\r' || ch == '\n') {
                if (command == "q") {
                    break;
                }
                if (command == "e") {
                    const std::string mergedQuery = mergeLines(query);
                    terminal.restore();
                    std::cout << "\r\n--- Executing Query ---\r\n" << std::flush;
                    if (mergedQuery.empty()) {
                        std::cout << "No query to execute.\n";
                    } else {
                        try {
                            compiler.query(const_cast<std::string &>(mergedQuery));
                        } catch (const std::exception &error) {
                            std::cerr << "Unexpected compiler error: " << error.what() << '\n';
                        } catch (...) {
                            std::cerr << "Unexpected compiler error.\n";
                        }
                        query.clear();
                    }
                    std::cout << "\nPress any key to continue..." << std::flush;
                    
                    // Re-enable raw mode to catch single key press
                    terminal.enable();
                    readSingleChar(ch);
                    std::cout << "\r\n" << std::flush;
                } else {
                    std::cout << "\r\nUnknown editor command: :" << command << "\r\n" << std::flush;
                }
                mode = Mode::Normal;
                printNormalPrompt();
            } else {
                command.push_back(ch);
                std::cout << ch << std::flush;
            }
        }
    }
}

} // namespace  <--- This bracket was the likely culprit!

int main(int argc, char *argv[]) {
    if (argc == 1) {
        runEditor(".");
        return 0;
    }

    const std::string argument = argv[1];
    if (argument == "--version" && argc == 2) {
        std::cout << VERSION << '\n';
        return 0;
    }
    if (argument == "--help" && argc == 2) {
        printUsage();
        return 0;
    }
    if (argument == "commands" && argc == 2) {
        printCommands();
        return 0;
    }
    if (argument == "command" && argc == 3) {
        printCommandHelp(argv[2]);
        return 0;
    }
    if (argc == 2) {
        runEditor(argument);
        return 0;
    }

    printUsage();
    return 2;
}