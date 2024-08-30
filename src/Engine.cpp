#include "Engine.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

using namespace chess;

Engine::Engine() {};
Engine::~Engine() {};

void Engine::Uci(std::vector<std::string> args) {
  args.resize(0);
  std::cout << "id name " << configuration.name << std::endl;
  std::cout << "id author " << configuration.author << std::endl;
  std::cout << "uciok" << std::endl;
};

void Engine::Stop(std::vector<std::string> args) {
  args.resize(0);
  stop = true;
};
void Engine::UnStop(std::vector<std::string> args) {
  args.resize(0);
  stop = false;
};
void Engine::DoNothing(std::vector<std::string> args) {
  while (!stop) {
    std::cout << args.front() << std::endl;
    sleep(1);
  }
};
