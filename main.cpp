#include <cassert>
#include <iostream>

#include <string>
#include <queue>

#define UNUSE(x) (void)(x)

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
	friend std::ostream& operator << (std::ostream& out, Command& cmd);
};

std::istream& operator >> (std::istream& in, Command& cmd) {
	in >> cmd.val;
	return in;
}

std::ostream& operator << (std::ostream& out, Command& cmd) {
	out << cmd.val;
	return out;
}

class CmdProc {
private:
	std::size_t n_inp_cmd;
	std::queue<Command> commands;
public:
	CmdProc(std::size_t n) : n_inp_cmd(n) {}
	~CmdProc() {}

	void poll(std::istream& in) {
		Command cmd{};

		for (auto i = 0; ; i++) {

			if (commands.size() == n_inp_cmd) {
				flush();
			}

			in >> cmd;

			if (cmd.IsOpenBlock()) {
				if (commands.size() != 0) {
					flush();
				}
				if (poll_dyn_block(in, cmd)) {
					flush();
					continue;
				}
				break;
			}

			if (cmd.IsEnd()) {
				flush();
				break;
			}

			commands.push(cmd);
		}
	}

private:

	bool poll_dyn_block(std::istream& in, Command& cmd) {
		for (auto i = 0; ; i++) {
			in >> cmd;

			if (cmd.IsOpenBlock()) {
				if (poll_dyn_block(in, cmd)) {
					continue;
				}
				return false;
			}

			if (cmd.IsEndBlock()) {
				return true;
			}

			if (cmd.IsEnd()) {
				return false;
			}

			commands.push(cmd);
		}
	}

	void flush() {
		std::cout << "bulk: ";
		while (commands.size() > 0) {
			std::cout << commands.front() << ',';
			commands.pop();
		}
		std::cout << '\b' << ' ' << std::endl;

	};
};


int main(int argc, char const* argv []) {
	UNUSE(argc);
	UNUSE(argv);

	CmdProc cmd_proc{ static_cast<std::size_t>(std::stoll(std::string(argv[1]))) };

	cmd_proc.poll(std::cin);

	return EXIT_SUCCESS;
}
