#include "UciHandler.hpp"
#include "Engine.hpp"
using namespace uci;

UciHandler::UciHandler(chess::Engine &_engine) {
  engine = _engine;
  commands["uci"] = chess::Engine::Uci;
}
UciHandler::~UciHandler() {}

void UciHandler::Listen(std::string command) {}
