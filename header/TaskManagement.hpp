#pragma once
#include "Engine.hpp"
#include "UciHandler.hpp"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

namespace taskManagement {
class TaskQueue {
  std::queue<std::function<void(chess::Engine &)>> queue;
  std::mutex mutex;
  std::condition_variable cv;

public:
  void push(std::function<void(chess::Engine &)> task);
  std::function<void(chess::Engine &)> pop();
};

void BackgroundWorker(TaskQueue &queue, chess::Engine &engine);

void ForegroundWorker(TaskQueue &queue, uci::UciHandler &uciHandler,
                      chess::Engine &engine);
} // namespace taskManagement

