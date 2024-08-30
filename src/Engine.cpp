#include "Engine.hpp"
#include <iostream>
#include <regex>
#include <unistd.h>

using namespace chess;

Engine::Engine() {};
Engine::~Engine() {};

void Engine::Uci(std::regex_iterator<std::string::iterator> &command) {
  std::cout << command->str() << std::endl;
  std::cout << "id name " << configuration.name << std::endl;
  std::cout << "id author " << configuration.author << std::endl;
  std::cout << "uciok" << std::endl;
};

void Engine::Stop(std::regex_iterator<std::string::iterator> &command) {
  std::cout << command->str() << std::endl;
  stop = true;
};
void Engine::UnStop(std::regex_iterator<std::string::iterator> &command) {
  std::cout << command->str() << std::endl;
  stop = false;
};
void Engine::DoNothing(std::regex_iterator<std::string::iterator> &command) {
  std::cout << command->str() << std::endl;
  while (!stop) {
    std::cout << "nothing" << std::endl;
    sleep(1);
  }
};
