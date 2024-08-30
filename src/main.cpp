#include "Engine.hpp"
#include "TaskManagement.hpp"
#include "UciHandler.hpp"
#include <thread>

int main() {
  chess::Engine engine;
  uci::UciHandler uciHandler;
  taskManagement::TaskQueue queue;
  std::thread bgThread(taskManagement::BackgroundWorker, std::ref(queue),
                       std::ref(engine));
  std::thread fgThread(taskManagement::ForegroundWorker, std::ref(queue));

  bgThread.join();
  fgThread.join();
  return 0;
}
