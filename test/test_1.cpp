#include <cassert>

#include "producer.hpp"
#include "consumer.hpp"

#define UNUSE(x) (void)(x)

using namespace pdc;
using namespace std::chrono_literals;

void spin() {
    while (true) {
        std::this_thread::sleep_for(100s);
    }
}

struct Message {
    std::size_t id = 0;
} msg;

auto TestProducer() {
    auto thread = std::thread([]() {
        Producer prod("pvt/position", 10);
        for (std::size_t i = 0; i < 50; i++) {
            msg.id = i;

            // Debug
            std::cout << "Generate message with ID - " << std::dec << msg.id << std::endl;

            prod.Publish(msg);

            std::this_thread::sleep_for(1ms);
        }
    });
    return thread;
}

auto TestConsumer() {
    auto thread = std::thread([]() {
        Consumer<Message> con{};
        con.Subscribe("pvt/position",
            [](Message msg) {
            // Debug
            std::cout << "Receive message with ID - " << std::dec << msg.id << std::endl;
        });

        // Spin
        spin();
    });
    return thread;
}

int main(int, char const**) {

    try {
        auto thread_cons = TestConsumer();
        auto thread_prod = TestProducer();

        thread_cons.join();
        thread_prod.join();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return EXIT_SUCCESS;
}
