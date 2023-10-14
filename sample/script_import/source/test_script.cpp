#include "test_script.hpp"

#include <iostream>

void TestScript::on_update(void) { std::cout << "on update!" << std::endl; }
void TestScript::on_spawn(void) { std::cout << "on spawn!" << std::endl; }

DLL EntityScript* create_instance(void) { return new TestScript; }