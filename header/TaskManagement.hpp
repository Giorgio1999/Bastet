#pragma once
#include "UciHandler.hpp"
#include <condition_variable>
#include <mutex>
#include <queue>

namespace taskManagement {
class TaskQueue {
  std::queue<std::string> queue;
  std::mutex mutex;
  std::condition_variable cv;

public:
  void push(std::string task);
  std::string pop();
};

void BackgroundWorker(TaskQueue &queue, uci::UciHandler &uciHandler);

void ForegroundWorker(TaskQueue &queue);
} // namespace taskManagement

