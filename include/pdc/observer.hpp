#pragma once

#include <vector>

class Observer {
public:
    virtual ~Observer() {};
    virtual void Update(std::vector<std::uint8_t> data) = 0;
};