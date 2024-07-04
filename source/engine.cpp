#include "engine.hpp"

std::unordered_map<std::string, std::shared_ptr<void>>
    SF::Engine::modules = {};

SF::Engine::Engine() { modules.clear(); }
SF::Engine::~Engine() { modules.clear(); }