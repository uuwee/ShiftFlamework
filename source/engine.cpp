#include "engine.hpp"

std::unordered_map<std::string, std::shared_ptr<void>>
    ShiftFlamework::Engine::modules = {};

ShiftFlamework::Engine::Engine() { modules.clear(); }
ShiftFlamework::Engine::~Engine() { modules.clear(); }