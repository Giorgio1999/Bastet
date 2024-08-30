#include "TaskManagement.hpp"
#include "UciHandler.hpp"
#include <iostream>
using namespace taskManagement;

void TaskQueue::push(std::string task) {
  std::lock_guard<std::mutex> lock(mutex);
  queue.push(task);
  cv.notify_one();
}

std::string TaskQueue::pop() {
  std::unique_lock<std::mutex> lock(mutex);
  cv.wait(lock, [this]() { return !queue.empty(); });
  auto task = queue.front();
  queue.pop();
  return task;
}

void taskManagement::BackgroundWorker(TaskQueue &queue,
                                      uci::UciHandler &uciHandler) {
  while (true) {
    std::string task = queue.pop();
    try {
      uciHandler.Listen(task);
    } catch (int e) {
      if (e == 1) {
        std::cout << "Unknown command" << std::endl;
      }
      if (e == 0) {
        std::exit(0);
      }
    }
  }
}

void taskManagement::ForegroundWorker(TaskQueue &queue,
                                      uci ::UciHandler &uciHandler) {
  std::string command;
  while (true) {
    if (std::getline(std::cin, command)) {
      if (command == "stop") {
        uciHandler.StopEngine();
      } else {
        queue.push(command);
        if (command == "quit") {
          std::exit(0);
        }
      }
    }
  }
}
