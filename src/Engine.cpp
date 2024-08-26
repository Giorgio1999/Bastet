#include "Engine.hpp"
#include <iostream>
#include <unistd.h>

using namespace chess;

Engine::Engine() {};
Engine::~Engine() {};

void Engine::Uci() {
  std::cout << "id name " << configuration.name << std::endl;
  std::cout << "id author " << configuration.author << std::endl;
  std::cout << "uciok" << std::endl;
};

void Engine::Stop() { stop = true; };
void Engine::UnStop() { stop = false; };
void Engine::DoNothing() {
  while (!stop) {
    std::cout << "nothing" << std::endl;
    sleep(1);
  }
};
