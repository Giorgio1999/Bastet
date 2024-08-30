#include "UciHandler.hpp"
#include "Engine.hpp"
#include <regex>
#include <vector>
using namespace uci;

UciHandler::UciHandler(chess::Engine &_engine) {
  engine = _engine;
  commands["uci"] = &chess::Engine::Uci;
  commands["stop"] = &chess::Engine::Stop;
  commands["unstop"] = &chess::Engine::UnStop;
  commands["doNothing"] = &chess::Engine::DoNothing;
}
UciHandler::~UciHandler() {}

void UciHandler::Listen(std::string command) {
  std::vector<std::string> args;
  std::regex re("\\S+");
  std::sregex_iterator it(command.begin(), command.end(), re);
  std::sregex_iterator end;
  std::string key = it->str();
  it++;
  while (it != end) {
    args.push_back(it->str());
    it++;
  }
  if (key == "quit") {
    throw 0;
  }
  if (commands.count(key) > 0) {
    commands[key](engine, args);
    commands["unstop"](engine, args);
  } else {
    throw 1;
  }
}

void UciHandler::StopEngine() {
  std::vector<std::string> args;
  commands["stop"](engine, args);
}

