#include <fstream>
#include <iostream>
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
std::string read_command_from_cin() {
    std::string cmd;
    std::cin >> cmd;
    return cmd;
}

std::string read_argument_from_cin() {
    std::string arg;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // чисто щоб пофіксить проблему з getline
    std::getline(std::cin, arg);
    return arg;
}

void add_task(std::vector<TaskObj>& tasks) {
    std::cout << "> ";
    const auto content = read_argument_from_cin();
    if (!content.empty())
        tasks.emplace_back(content);
}

void remove_task(std::vector<TaskObj>& tasks) {
    const auto id = std::stoi(read_command_from_cin());
    if (id > tasks.size() || id < 0)
        throw std::runtime_error("Task ID out of range");
    tasks.erase(tasks.begin() + id);
}

///Зберігає завдання за вказаним шляхом
void save_tasks(const std::string& path, const std::vector<TaskObj>& tasks) {
    json j;
    to_json(j, tasks);
    write_json_to_file(j, path);
}

///Завантажує завдання за вказаним шляхом
void load_tasks(const std::string& path, std::vector<TaskObj>& tasks) {
    json j = json::parse(read_file(path));
    from_json(j, tasks);
}

int main() {
    //включає флаг (bool alphabetic) для стандартного потоку виводу
    std::cout << std::boolalpha;

    std::string current_path = DEFAULT_PATH;
    bool isRunning = true;
    std::vector<TaskObj> tasks;

    //Перше завантаження по дефолтному шляху
    load_tasks(current_path, tasks);

    //Основний цикл взаємодії із користувачем
    while (isRunning) {
        try {
            //Обробка команд
            const auto cmd = read_command_from_cin();
            if (cmd == "exit")
                isRunning = false;
            else if (cmd == "add") {
                add_task(tasks);
            } else if (cmd == "remove") {
                remove_task(tasks);
            } else if (cmd == "show_all") {
                print_tasks(tasks);
            }
        } catch (std::runtime_error& e) {
            std::cout << e.what() << std::endl;
        }
    }

    //Зберігання завдань при виході із програми
    save_tasks(current_path, tasks);

    return 0;
}