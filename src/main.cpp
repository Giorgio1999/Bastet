#include "Engine.hpp"
#include "TaskManagement.hpp"
#include "UciHandler.hpp"
#include <thread>

int main() {
  chess::Engine engine;
  uci::UciHandler uciHandler(engine);
  taskManagement::TaskQueue queue;
  std::thread bgThread(taskManagement::BackgroundWorker, std::ref(queue),
                       std::ref(uciHandler));
  std::thread fgThread(taskManagement::ForegroundWorker, std::ref(queue),
                       std::ref(uciHandler));

  bgThread.join();
  fgThread.join();
  return 0;
}
