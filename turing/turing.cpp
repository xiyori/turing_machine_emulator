#include "turing.h"
#include <fstream>
#include <filesystem>
#include <cstdio>

const vector<string> man_list_commands = {
    "man: an interface to command manual pages",
    "list: show all existing machines",
    "define: create or edit machine",
    "rename: rename machine",
    "run: compile and execute machine",
    "pass: do nothing",
    "exit: stop application"
};

const unordered_map<string, string> man_commands = {
    {"define", "DEFINE                                              IO Commands\n\n"
          "NAME\n       define - create or edit machine\n\n"
          "SYNTAX\n       {d | define} NAME\n\n"
          "DESCRIPTION\n       Open machine listing file in Notepad/Notepad++ if installed.\n\n"},
    {"run", "RUN                                               Execution Commands\n\n"
          "NAME\n       run - compile and execute machine\n\n"
          "SYNTAX\n       {r | run} NAME INPUT [OPTIONS]\n\n"
          "DESCRIPTION\n       Load machine from file and process input. Note that "" is processed as void input! If Turing machine doesn't stop, kills it after iterations limit is exceeded.\n\n"
          "       -f, --fast\n              no animation, only output (the default)\n\n"
          "       -s, --seq[=TIME]\n              animated steps, time interval is TIME ms (default 500)\n\n"
          "       -m, --man\n              awaiting manual input after each step\n\n"
          "       --lim=LIMIT\n              sets iterations limit (default 1E+8)\n\n"},
    {"pass", "PASS                                              Useless Commands\n\n"
          "NAME\n       pass - do nothing\n\n"
          "SYNTAX\n       {p | pass}\n\n"
          "DESCRIPTION\n       Do literally nothing.\n\n" },
    {"exit", "EXIT                                           Application Commands\n\n"
          "NAME\n       exit - stop application\n\n"
          "SYNTAX\n       {e | exit}\n\n"
          "DESCRIPTION\n       Exit app.\n\n"},
    {"man", "MAN                                           Application Commands\n\n"
          "NAME\n       man - an interface to command manual pages\n\n"
          "SYNTAX\n       {m | man} [COMMAND]\n\n"
          "DESCRIPTION\n       List all available commands. If an argument is provided, display specific command manual page.\n\n"},
    {"rename", "RENAME                                           IO Commands\n\n"
          "NAME\n       rename - rename machine\n\n"
          "SYNTAX\n       {rn | rename} OLD_NAME NEW_NAME\n\n"
          "DESCRIPTION\n       Rename machine file.\n\n"},
    {"list", "LIST                                           IO Commands\n\n"
          "NAME\n       list - show all existing machines\n\n"
          "SYNTAX\n       {ls | list}\n\n"
          "DESCRIPTION\n       List all machine names.\n\n"} };

struct Command {
    vector<string> arguments;

    Command() : arguments({ "pass" }) {}
    Command(string& command_text) {
        if (command_text.empty()) {
            arguments.push_back("pass");
            return;
        }
        command_text += " ";
        size_t pos = 0;
        while ((pos = command_text.find(" ")) != string::npos) {
            arguments.push_back(command_text.substr(0, pos));
            command_text.erase(0, pos + 1);
            while (!command_text.empty() && command_text[0] == ' ')
                command_text.erase(0, 1);
        }
    }

    const string& Name() const {
        return arguments[0];
    }

    const string& Get(int index) const {
        return arguments[index];
    }

    int Count() const {
        return arguments.size() - 1;
    }
};

void EditMachine(const string& name) {
    std::filesystem::path cwd = std::filesystem::current_path() / (name + ".txt");
    system(&("\"\"C:\\Program Files (x86)\\Notepad++\\notepad++.exe\" \"" + cwd.string() + "\"\"")[0]);
}

int sleep_time;

void RunMachine(const Machine& turing, const string& input, const string& mode, int limit = 1E+8) {
    void(*func)() = []() {};
    bool fast = false;
    if (mode.find("-s") == 0 || mode.find("--seq") == 0) {
        sleep_time = 500;
        if (mode.find("-s=") == 0) {
            Print("Wrong options!\n");
            throw std::exception();
        }
        if (mode.find("=") == 5)
            sleep_time = std::stoi(mode.substr(6));
        func = []() {
            Sleep(sleep_time);
        };
    } else if (mode == "-m" || mode == "--man") {
        func = []() {
            PrintLightGrayBack("Press Enter to continue");
            std::cin.ignore();
        };
    } else if (mode == "-f" || mode == "--fast") {
        fast = true;
    } else {
        Print("Wrong options!\n");
        throw std::exception();
    }
    turing.Process(input == "\"\"" ? "" : input, func, fast, limit);
}

Machine LoadMachine(const string& name) {
    std::ifstream fin(name + ".txt");
    if (!fin.good()) {
        PrintRed("Machine " + name + " does not exist!\n");
        throw std::exception();
    }
    string line;
    vector<string> list;
    while (std::getline(fin, line)) {
        list.push_back(line);
    }
    return Machine(list);
}

int main() {
    //PrintRainbow("wubba lubba dub dub");
    Print("TURING TERMINAL 1.0.0\nSergejVolkov 2020\n\n");
    string command_text;
    while (true) {
        PrintGreen("turing@Machine");
        Print(":");
        PrintBlue("~");
        Print("/bin");
        PrintGray("$ ");
        std::getline(std::cin, command_text);
        Command command(command_text);
        if (command.Name() == "pass" || command.Name() == "p") {
        } else if (command.Name() == "list" || command.Name() == "ls") {
            for (const auto & entry : std::filesystem::directory_iterator("")) {
                string name = entry.path().string();
                if (name.find(".txt") != string::npos)
                    std::cout << name.substr(0, name.size() - 4) << '\n';
            }
        } else if (command.Name() == "define" || command.Name() == "d") {
            if (command.Count() == 1) {
                EditMachine(command.Get(1));
            } else {
                PrintRed(command.Name() + ": wrong arguments\n");
            }
        } else if (command.Name() == "rename" || command.Name() == "rn") {
            if (command.Count() == 2) {
                std::ifstream fin(command.Get(1) + ".txt");
                if (fin.good()) {
                    fin.close();
                    std::rename(&(command.Get(1) + ".txt")[0], &(command.Get(2) + ".txt")[0]);
                } else {
                    PrintRed("Machine " + command.Get(1) + " does not exist!\n");
                }
            } else {
                PrintRed(command.Name() + ": wrong arguments\n");
            }
        } else if (command.Name() == "run" || command.Name() == "r") {
            try {
                Machine turing = LoadMachine(command.Get(1));
                if (command.Count() == 2) {
                    RunMachine(turing, command.Get(2), "-f");
                } else if (command.Count() == 3) {
                    if (command.Get(3).find("--lim=") == 0) {
                        RunMachine(turing, command.Get(2), "-f", std::stoi(command.Get(3).substr(6)));
                    } else {
                        RunMachine(turing, command.Get(2), command.Get(3));
                    }
                } else if (command.Count() == 4) {
                    if (command.Get(3).find("--lim=") == 0) {
                        RunMachine(turing, command.Get(2), command.Get(4), std::stoi(command.Get(3).substr(6)));
                    } else {
                        RunMachine(turing, command.Get(2), command.Get(3), std::stoi(command.Get(4).substr(6)));
                    }
                } else {
                    PrintRed(command.Name() + ": wrong arguments\n");
                }
            } catch (...) {
                PrintRed(command.Name() + ": something went wrong, please try again\n");
            }
        } else if (command.Name() == "man" || command.Name() == "m") {
            if (command.Count() == 0) {
                std::cout << "What manual page do you want?\n"
                    "For example, try 'man man'.\n\n";
                PrintBlue("Available commands:\n");
                for (const auto& line : man_list_commands)
                    std::cout << line << '\n';
            } else if (command.Count() == 1) {
                auto iter = man_commands.find(command.Get(1));
                if (iter == man_commands.end()) {
                    std::cout << "No manual entry for " << command.Get(1) << '\n';
                } else {
                    ClearScreen();
                    std::cout << iter->second;
                    PrintLightGrayBack("Press Enter to exit manual");
                    std::cin.ignore();
                    ClearScreen();
                }
            } else {
                PrintRed(command.Name() + ": wrong arguments\n");
            }
        } else if (command.Name() == "exit" || command.Name() == "e") {
            return 0;
        } else {
            PrintRed(command.Name() + ": command not found\n");
        }
    }
    return 0;
}
