#pragma once
#include <string>
#include <vector>

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

  void Uci(std::vector<std::string> args);
  void Stop(std::vector<std::string> args);
  void UnStop(std::vector<std::string> args);
  void DoNothing(std::vector<std::string> args);
};

} // namespace chess
