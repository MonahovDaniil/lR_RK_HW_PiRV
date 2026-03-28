#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <random>
#include <mutex>
#include <chrono>
#include <memory>
#include <atomic>

struct Task {
    int id;
    int priority;
    int duration;
};

struct Compare {
    bool operator()(const Task& a, const Task& b) {
        return a.priority > b.priority;
    }
};

struct Server {
    int id;
    std::priority_queue<Task, std::vector<Task>, Compare> tasks;
    std::mutex mtx;

    Server() : id(0) {}
};

std::vector<std::unique_ptr<Server>> servers;
std::mutex global_mtx;
std::mutex cout_mtx;
std::atomic<int> completedTasks{0};

int taskCounter = 0;

void add_server() {
    std::lock_guard<std::mutex> lock(global_mtx);

    servers.push_back(std::make_unique<Server>());
    servers.back()->id = servers.size();

    std::lock_guard<std::mutex> out(cout_mtx);
    std::cout << "Добавлен новый сервер! ID: "
              << servers.back()->id << "\n";
}

void generate_tasks(int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> prio(1, 3);
    std::uniform_int_distribution<> time(1, 5);

    for (int i = 0; i < count; i++) {
        Task t = {++taskCounter, prio(gen), time(gen)};

        int chosen;

        {
            std::lock_guard<std::mutex> lock(global_mtx);
            std::uniform_int_distribution<> server_dist(0, servers.size() - 1);
            chosen = server_dist(gen);

            std::lock_guard<std::mutex> lock2(servers[chosen]->mtx);
            servers[chosen]->tasks.push(t);
        }

        std::lock_guard<std::mutex> out(cout_mtx);
        std::cout << "Задача " << t.id << " добавлена на сервер "
                  << servers[chosen]->id
                  << " (приоритет " << t.priority << ")\n";
    }
}

void balance_load() {
    std::lock_guard<std::mutex> lock(global_mtx);

    for (int i = 0; i < servers.size(); i++) {
        for (int j = 0; j < servers.size(); j++) {
            if (i == j) continue;

            std::scoped_lock lock(servers[i]->mtx, servers[j]->mtx);

            if (servers[i]->tasks.size() > 3 &&
                servers[j]->tasks.size() < 2) {

                Task t = servers[i]->tasks.top();
                servers[i]->tasks.pop();
                servers[j]->tasks.push(t);

                std::lock_guard<std::mutex> out(cout_mtx);
                std::cout << "Задача " << t.id
                          << " перенесена с сервера "
                          << servers[i]->id
                          << " на сервер "
                          << servers[j]->id << "\n";

                break;
            }
        }
    }
}

void server_worker(int index) {
    while (true) {
        Task task;
        bool hasTask = false;
        Server* srv;

        {
            std::lock_guard<std::mutex> lock(global_mtx);
            if (index >= servers.size()) return;
            srv = servers[index].get();
        }

        {
            std::lock_guard<std::mutex> lock(srv->mtx);
            if (!srv->tasks.empty()) {
                task = srv->tasks.top();
                srv->tasks.pop();
                hasTask = true;
            }
        }

        if (!hasTask) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        {
            std::lock_guard<std::mutex> out(cout_mtx);
            std::cout << "Сервер " << srv->id
                      << " выполняет задачу " << task.id
                      << " (приоритет " << task.priority << ")\n";
        }

        std::this_thread::sleep_for(std::chrono::seconds(task.duration));

        {
            std::lock_guard<std::mutex> out(cout_mtx);
            std::cout << "Задача " << task.id
                      << " выполнена сервером "
                      << srv->id << "\n";
            completedTasks++;
        }
    }
}

void monitor() {
    while (true) {
        int totalTasks = 0;
        int serverCount = 0;

        {
            std::lock_guard<std::mutex> lock(global_mtx);

            serverCount = servers.size();

            for (auto& s : servers) {
                std::lock_guard<std::mutex> lock2(s->mtx);
                totalTasks += s->tasks.size();
            }
        }

        if (serverCount == 0) continue;

        double load = (double)totalTasks / serverCount;

        if (load > 0.8) {
            add_server();

            std::lock_guard<std::mutex> lock(global_mtx);
            std::thread(server_worker, servers.size() - 1).detach();
        }

        balance_load();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    for (int i = 0; i < 5; i++)
        add_server();

    for (int i = 0; i < servers.size(); i++) {
        std::thread(server_worker, i).detach();
    }

    std::thread(monitor).detach();

    generate_tasks(20);

    std::this_thread::sleep_for(std::chrono::seconds(20));
    std::cout << "Всего выполнено задач: " << completedTasks << "\n";

    return 0;
}