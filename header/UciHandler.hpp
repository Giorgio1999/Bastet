#pragma once
#include "Engine.hpp"
#include <functional>
#include <regex>
#include <string>
#include <unordered_map>

namespace uci {
class UciHandler {
public:
  std::unordered_map<
      std::string,
      std::function<void(chess::Engine &,
                         std::regex_iterator<std::string::iterator> &)>>
      commands;

  UciHandler();
  ~UciHandler();
  std::function<void(chess::Engine &,
                     std::regex_iterator<std::string::iterator> &)>
  Listen(std::string command);
};

} // namespace uci
