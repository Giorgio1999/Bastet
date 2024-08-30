#pragma once
#include <regex>
#include <string>

namespace chess {

struct Configuration {
  std::string name = "Bastet";
  std::string author = "Giorgio Lovato";
};

class Engine {
public:
  bool stop = false;
  Configuration configuration;

  Engine();
  ~Engine();

  void Uci(std::regex_iterator<std::string::iterator> &command);
  void Stop(std::regex_iterator<std::string::iterator> &command);
  void UnStop(std::regex_iterator<std::string::iterator> &command);
  void DoNothing(std::regex_iterator<std::string::iterator> &command);
};

} // namespace chess
