#include "UciHandler.hpp"
#include "Engine.hpp"
#include <functional>
#include <string>
#include <unordered_map>
using namespace uci;

UciHandler::UciHandler() {
  commands["uci"] = &chess::Engine::Uci;
  commands["donothing"] = &chess::Engine::DoNothing;
}
UciHandler::~UciHandler() {}

std::function<void(chess::Engine &)> UciHandler::Listen(std::string command) {
  if (command == "quit") {
    throw 1;
  }
  if (command == "stop") {
    throw 2;
  }
  if (commands.find(command) == commands.end()) {
    throw 0;
  }
  return commands[command];
}
