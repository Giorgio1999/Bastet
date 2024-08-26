#pragma once
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

  void Uci();
  void Stop();
  void UnStop();
  void DoNothing();
};

} // namespace chess
