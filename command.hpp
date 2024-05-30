#pragma once

#include <string>

class Command {
private:
	std::string val;
public:
	Command() = default;
	~Command() = default;

	bool IsOpenBlock() const {
		return val[0] == '{' && val.size() == 1;
	}

	bool IsEndBlock() const {
		return val[0] == '}' && val.size() == 1;
	}

	bool IsEnd() const {
		return val == "EOF" && val.size() == 3;
	}

	friend std::istream& operator >> (std::istream& in, Command& cmd);
	friend std::ostream& operator << (std::ostream& out, const Command& cmd);
};

std::istream& operator >> (std::istream& in, Command& cmd) {
	in >> cmd.val;
	return in;
}

std::ostream& operator << (std::ostream& out, const Command& cmd) {
	out << cmd.val;
	return out;
}
