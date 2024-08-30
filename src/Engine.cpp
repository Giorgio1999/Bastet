#include "Engine.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

using namespace chess;

Engine::Engine() {};
Engine::~Engine() {};

void Engine::Uci(std::vector<std::string> args) {
  std::cout << args.size() << std::endl;
  std::cout << "id name " << configuration.name << std::endl;
  std::cout << "id author " << configuration.author << std::endl;
  std::cout << "uciok" << std::endl;
};

void Engine::Stop(std::vector<std::string> args) {
  std::cout << args.size() << std::endl;
  stop = true;
};
void Engine::UnStop(std::vector<std::string> args) {
  std::cout << args.size() << std::endl;
  stop = false;
};
void Engine::DoNothing(std::vector<std::string> args) {
  std::cout << args.size() << std::endl;
  while (!stop) {
    std::cout << "nothing" << std::endl;
    sleep(1);
  }
};
