#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

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

void to_json(json &j, std::vector<TaskObj> &objs) {
    for (const auto& obj : objs) {
        const char *str_id = std::to_string(obj.id).c_str();
        j[str_id]["flag"] = obj.flag;
        j[str_id]["content"] = obj.content;
    }
}

void from_json(const json &j, std::vector<TaskObj> &objs) {
    for (int i = 1; true ; ++i) {
        const char *str_id = std::to_string(i).c_str();
        try {
            auto content = j.at(str_id)["content"].get<std::string>();
            auto flag = j.at(str_id)["flag"].get<bool>();
            objs.push_back(TaskObj(i, content, flag));
        } catch (json::out_of_range& exception) {
            break;
        }
    }
}

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

void write_json_to_file(const json& j, const std::string& path, const int indent = 4) {
    std::ofstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Could not open file " + path);
    file << j.dump(indent);
    file.close();
}

void print_task(const TaskObj& task) {
    std::cout << task.id << " " << task.content << "|" << task.flag << std::endl;
}

void print_tasks(std::vector<TaskObj>::const_iterator tasks_begin, std::vector<TaskObj>::const_iterator tasks_end) {
    for (auto i = tasks_begin; i != tasks_end; ++i) {
        print_task(*i);
    }
}

int main() {
    //enable alphabetic output of bool values
    std::cout << std::boolalpha;

    std::vector<TaskObj> tasks;

    return 0;
};