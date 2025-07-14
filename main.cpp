#include <fstream>
#include <iostream>
#include <set>
#include <nlohmann/json.hpp>

///Структура завдання
struct TaskObj {
    bool flag;
    std::string content;

    explicit TaskObj(const std::string& content, const bool flag = false) {
        this->flag = flag;
        this->content = content;
    }
};

using json = nlohmann::json;

///Серіалізація завдань з вектора objs в json
void to_json(json &j, const std::vector<TaskObj> &objs) {
    int id = 0;
    for (const auto& obj : objs) {
        const char *str_id = std::to_string(id).c_str();
        j[str_id]["flag"] = obj.flag;
        j[str_id]["content"] = obj.content;
        ++id;
    }
}

///Десеріалізація завдань з json в вектор objs
void from_json(const json &j, std::vector<TaskObj> &objs) {
    for (const auto& [string_id, val] : j.items()) {
        auto content = val.at("content").get<std::string>();
        auto flag = val.at("flag").get<bool>();
        objs.emplace_back(content, flag);
    }
}

///Читає і повертає вміст файлу за вказаним шляхом
std::string read_file(const std::string& path) {
    std::string result, line;
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Could not open file " + path);
    while (std::getline(file, line)) {
        result += line + "\n";
    }
    file.close();
    return result;
}

///Записує json об'єкт в файл за вказаним шляхом, з вказаним відступом (4 за умовчанням)
void write_json_to_file(const json& j, const std::string& path, const int indent = 4) {
    std::ofstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Could not open file " + path);
    file << j.dump(indent);
    file.close();
}

///Виводить на екран об'єкт завдання
void print_task(const TaskObj& task, const int id) {
    std::cout << id << ": " << task.content << "|" << task.flag << std::endl;
}

///Виводить на екран указаний проміжок завдань
void print_tasks(const std::vector<TaskObj>& tasks) {
    for (int i = 0; i < tasks.size(); ++i) {
        print_task(tasks[i], i);
    }
}

///Читає задані команди з консолі та повертає об'єкт pair з first - значенням команди, second - аргументом
///Якщо команда не знаходиться в списку commands, то повертається pair із пустими значеннями
std::pair<std::string, std::string> read_command_from_cin(const std::set<std::string>& available_commands) {
    std::string cmd;
    std::cin >> cmd;
    if (available_commands.contains(cmd)) {
        std::string arg;
        std::cin >> arg;
        return std::make_pair(cmd, arg);
    }
    return std::make_pair("", "");
}

void add_task(const std::string& content, std::vector<TaskObj>& tasks) {
    if (content.empty())
        throw std::runtime_error("Task content cannot be empty");
    tasks.emplace_back(content);
}

void remove_task(const int id, std::vector<TaskObj>& tasks) {
    if (id >= tasks.size() && id < 0)
        throw std::runtime_error("Task id out of range");
    tasks.erase(tasks.begin() + id);
}

int main() {
    //включає флаг (bool alphabetic) для стандартного потоку виводу
    std::cout << std::boolalpha;

    bool isRunning = true;
    std::vector<TaskObj> tasks;
    json jsonObject;
    const std::set<std::string> available_commands = {
        "show_all", "exit", "add", "remove", "show", "save",
        "mark"
    };

    while (isRunning) {
        try {
            auto cmd_pair = read_command_from_cin(available_commands);
            if (cmd_pair.first == "exit")
                isRunning = false;
            else if (cmd_pair.first == "add") {
                add_task(cmd_pair.second, tasks);
            } else if (cmd_pair.first == "remove") {
                const auto id = std::stoi(cmd_pair.second);
                remove_task(id, tasks);
            } else if (cmd_pair.first == "show_all") {
                print_tasks(tasks);
            }
        } catch (std::runtime_error& e) {
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}