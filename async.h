#pragma once

#include <cstddef>
#include <memory>

class Context;

std::shared_ptr<Context> connect(std::size_t num_cmd);
void receive(const char* str, std::size_t len, std::shared_ptr<Context>& context);
void disconnect(std::shared_ptr<Context>& context);