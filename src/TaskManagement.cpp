#include "TaskManagement.hpp"
#include "Engine.hpp"
#include "UciHandler.hpp"
#include <atomic>
#include <iostream>
#include <regex>
using namespace taskManagement;
std::atomic<bool> running = true;

void TaskQueue::push(std::function<void(chess::Engine &)> task) {
  std::lock_guard<std::mutex> lock(mutex);
  queue.push(task);
  cv.notify_one();
}

std::function<void(chess::Engine &)> TaskQueue::pop() {
  std::unique_lock<std::mutex> lock(mutex);
  cv.wait(lock, [this]() { return !queue.empty(); });
  auto task = queue.front();
  queue.pop();
  return task;
}

void taskManagement::BackgroundWorker(TaskQueue &queue, chess::Engine &engine) {
  while (running) {
    std::function<void(chess::Engine &)> task = queue.pop();
    try {
      task(engine);
    } catch (const int &e) {
      if (e == 0) {
        running = false;
        std::exit(0);
      }
    }
    engine.UnStop();
  }
}

void taskManagement::ForegroundWorker(TaskQueue &queue,
                                      uci::UciHandler &uciHandler,
                                      chess::Engine &engine) {
  std::string command;
  std::function<void(chess::Engine &)> callable;
  std::regex format("\\S+");
  std::smatch match;
  while (running) {
    if (std::getline(std::cin, command)) {
      std::regex_iterator<std::string::iterator> rit(command.begin(),
                                                     command.end(), format);
      std::regex_iterator<std::string::iterator> rend;
      if (rit == rend) {
        continue;
      }
      std::string key = rit->str();
      try {
        callable = uciHandler.Listen(key);
      } catch (const int &e) {
        if (e == 0) {
          std::cerr << "UciHandler: invalid command: " << key << std::endl;
          continue;
        }
        if (e == 1) {
          running = false;
          std::exit(0);
        }
        if (e == 2) {
          engine.Stop();
          callable = nullptr;
        }
      }
      if (callable != nullptr) {
        queue.push(std::ref(callable));
      }
    }
  }
}
