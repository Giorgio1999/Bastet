#include "TaskManagement.hpp"
#include "UciHandler.hpp"
#include <atomic>
#include <iostream>
using namespace taskManagement;
std::atomic<bool> running = true;

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
  while (running) {
    std::string task = queue.pop();
    try {
      uciHandler.Listen(task);
    } catch (int e) {
    }
  }
}

void taskManagement::ForegroundWorker(TaskQueue &queue) {
  std::string command;
  while (running) {
    if (std::getline(std::cin, command)) {
      queue.push(command);
      if (command == "quit") {
        std::exit(0);
      }
    }
  }
}
