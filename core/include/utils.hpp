#pragma once
#include <exception>
#include <iostream>

//utility function for pretty printing nested exceptions
void print_nested_exception(const std::exception& e, int level = 0);