#pragma once
#include "Engine.hpp"
#include <functional>
#include <string>
#include <unordered_map>

namespace uci {
class UciHandler {
public:
  std::unordered_map<std::string, std::function<void(chess::Engine &)>>
      commands;

  UciHandler();
  ~UciHandler();
  std::function<void(chess::Engine &)> Listen(std::string command);
};

} // namespace uci
