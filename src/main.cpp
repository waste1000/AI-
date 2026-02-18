// Simple C++ PoC simulator for AIC-Kali message passing (very small)
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <map>
#include <vector>
#include <string>

struct Message {
    int sender_id;
    int target_id;
    std::string method;
    int request_id;
    int priority;
};

struct Agent {
    int id;
    std::string name;
    std::queue<Message> inbox;
    std::mutex mtx;
    std::condition_variable cv;
    virtual void handle(Message msg) { }
};

struct KernelSim {
    std::map<int, Agent*> agents;
    std::mutex global_mtx;
    std::vector<Message> global_queue;
    int next_agent_id = 1;
    int next_request_id = 1;

    int register_agent(Agent* a) {
        std::lock_guard<std::mutex> lk(global_mtx);
        a->id = next_agent_id++;
        agents[a->id] = a;
        return a->id;
    }

    void post(Message m) {
        std::lock_guard<std::mutex> lk(global_mtx);
        global_queue.push_back(m);
        // simple priority sort
        std::sort(global_queue.begin(), global_queue.end(), [](const Message& a, const Message& b){ return a.priority < b.priority; });
    }

    void run_once() {
        std::lock_guard<std::mutex> lk(global_mtx);
        if (global_queue.empty()) return;
        Message m = global_queue.front();
        global_queue.erase(global_queue.begin());
        auto it = agents.find(m.target_id);
        if (it != agents.end()) {
            Agent* tgt = it->second;
            {
                std::lock_guard<std::mutex> lk2(tgt->mtx);
                tgt->inbox.push(m);
            }
            tgt->cv.notify_one();
        }
    }
};

int main() {
    KernelSim ks;
    std::cout << "KernelSim started" << std::endl;
    // Further PoC can create agents and simulate messaging
    return 0;
}