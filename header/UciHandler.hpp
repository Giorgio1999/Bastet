#pragma once
#include "Engine.hpp"
#include <functional>
#include <string>
#include <unordered_map>

namespace uci {
class UciHandler {
public:
  chess::Engine engine;
  std::unordered_map<
      std::string,
      std::function<void(chess::Engine &engine, std::vector<std::string> args)>>
      commands;
  UciHandler(chess::Engine &_engine);
  ~UciHandler();
  void Listen(std::string command);
};

} // namespace uci
