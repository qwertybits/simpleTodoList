#include <fstream>
#include <iostream>
#include <set>
#include <nlohmann/json.hpp>

///Структура завдання
struct TaskObj {
    unsigned id;
    bool flag;
    std::string content;

    TaskObj(const unsigned id, const std::string& content, const bool flag = false) {
        this->id = id;
        this->flag = flag;
        this->content = content;
    }
};

using json = nlohmann::json;

///Серіалізація завдань з вектора objs в json
void to_json(json &j, const std::vector<TaskObj> &objs) {
    for (const auto& obj : objs) {
        const char *str_id = std::to_string(obj.id).c_str();
        j[str_id]["flag"] = obj.flag;
        j[str_id]["content"] = obj.content;
    }
}

///Десеріалізація завдань з json в вектор objs
void from_json(const json &j, std::vector<TaskObj> &objs) {
    for (const auto& [string_id, val] : j.items()) {
        auto id = std::stoi(string_id);
        auto content = val.at("content").get<std::string>();
        auto flag = val.at("flag").get<bool>();
        objs.emplace_back(id, content, flag);
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
void print_task(const TaskObj& task) {
    std::cout << task.id << " " << task.content << "|" << task.flag << std::endl;
}

///Виводить на екран указаний проміжок завдань
void print_tasks(std::vector<TaskObj>::const_iterator tasks_begin, std::vector<TaskObj>::const_iterator tasks_end) {
    for (auto i = tasks_begin; i != tasks_end; ++i) {
        print_task(*i);
    }
}

///Читає задані команди з консолі та повертає об'єкт pair з first - значенням команди, second - аргументом
///Якщо команда не знаходиться в списку commands, то повертається pair із пустими значеннями
std::pair<std::string, std::string> read_command_from_cin(const std::set<std::string>& commands) {
    std::string cmd;
    std::cin >> cmd;
    if (commands.contains(cmd)) {
        std::string arg;
        std::cin >> arg;
        return std::make_pair(cmd, arg);
    }
    return std::make_pair("", "");
}

int main() {
    //включає флаг (bool alphabetic) для стандартного потоку виводу
    std::cout << std::boolalpha;

    const std::set<std::string> cmds = {"add", "remove", "mark", "exit", "save"};

    std::vector<TaskObj> tasks;

    const auto command_pair = read_command_from_cin(cmds);
    std::cout << command_pair.first << " " << command_pair.second << std::endl;

    return 0;
};