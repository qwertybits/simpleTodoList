#include <fstream>
#include <iostream>
#include <set>
#include <nlohmann/json.hpp>

constexpr std::string DEFAULT_PATH = "task.json";

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

///Записує вміст у файл за вказаним шляхом
void write_to_file(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Could not open file " + path);
    file << content;
    file.close();
}

///Виводить в консоль об'єкт завдання
void print_task(const TaskObj& task, const int id) {
    std::cout << id << "[" << (task.flag ? "X" : " ") << "] " << task.content << std::endl;
}

///Виводить в консоль завдання
void print_tasks(const std::vector<TaskObj>& tasks) {
    for (int i = 0; i < tasks.size(); ++i) {
        print_task(tasks[i], i);
    }
    if (tasks.empty()) {
        std::cout << "No tasks" << std::endl;
    }
}

///Читає задані команди з консолі та повертає об'єкт pair з first - значенням команди, second - аргументом
///Якщо команда не знаходиться в списку commands, то повертається pair із пустими значеннями
std::pair<std::string, std::string> read_command_from_cin(const std::set<std::string>& commandsWithArgument) {
    std::string cmd;
    std::cin >> cmd;
    if (commandsWithArgument.contains(cmd)) {
        std::string argument;
        std::cin >> argument;
        return std::make_pair(cmd, argument);
    }
    return std::make_pair(cmd, "");
}

///Додає завдання в список
void add_task(std::vector<TaskObj>& tasks) {
    std::cout << "> ";
    std::string content;
    std::getline(std::cin, content);
    if (!content.empty())
        tasks.emplace_back(content);
}

///Видаляє завдання з списку по id
void remove_task(std::vector<TaskObj>& tasks, const std::string& argument) {
    const auto id = std::stoi(argument);
    if (id > tasks.size() || id < 0)
        throw std::runtime_error("Task ID out of range");
    tasks.erase(tasks.begin() + id);
}

///Зберігає завдання за вказаним шляхом
void save_tasks(const std::string& path, const std::vector<TaskObj>& tasks) {
    json j;
    to_json(j, tasks);
    write_to_file(path, j.dump(4));
}

///Завантажує завдання за вказаним шляхом
void load_tasks(const std::string& path, std::vector<TaskObj>& tasks) {
    json j = json::parse(read_file(path));
    from_json(j, tasks);
}

void mark_task(std::vector<TaskObj>& tasks, const std::string& argument) {
    const auto id = std::stoi(argument);
    if (id > tasks.size() || id < 0)
        throw std::runtime_error("Task ID out of range");
    tasks.at(id).flag = !tasks.at(id).flag;
}

int main() {
    //включає флаг (bool alphabetic) для стандартного потоку виводу
    std::cout << std::boolalpha;

    std::string current_path = DEFAULT_PATH;
    bool isRunning = true;
    std::vector<TaskObj> tasks;
    const std::set<std::string> commandsWithArgument = {"remove", "mark"};

    //Перше завантаження по дефолтному шляху
    load_tasks(current_path, tasks);

    //Основний цикл взаємодії із користувачем
    while (isRunning) {
        try {
            //Обробка команд
            const auto cmd = read_command_from_cin(commandsWithArgument);
            if (cmd.first == "exit")
                isRunning = false;
            else if (cmd.first == "add") {
                add_task(tasks);
            } else if (cmd.first == "remove") {
                remove_task(tasks, cmd.second);
            } else if (cmd.first == "show_all") {
                print_tasks(tasks);
            } else if (cmd.first == "mark") {
                mark_task(tasks, cmd.second);
            } else {
                std::cout << "Unknown command: " << cmd.first << std::endl;
            }
        } catch (std::runtime_error& e) {
            std::cout << e.what() << std::endl;
        }
    }
    //Зберігання завдань при виході із програми
    save_tasks(current_path, tasks);

    return 0;
}