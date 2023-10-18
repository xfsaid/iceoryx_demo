#include <chrono>
#include <iceoryx/v/iceoryx_posh/popo/publisher.hpp>
#include <iceoryx_dust/posix_wrapper/signal_watcher.hpp>
#include <iostream>
#include <thread>

struct Point
{
    int x;
};

int main()
{
    iox::runtime::PoshRuntime::initRuntime("test_cb_pub");

    iox::popo::PublisherOptions options;
    options.nodeName = "Pub_Node_With_Options";
    options.subscriberTooSlowPolicy = iox::popo::ConsumerTooSlowPolicy::WAIT_FOR_CONSUMER;
    iox::popo::Publisher<Point> publisher({"aa", "bb", "cc"}, options);

    publisher.offer();
    while (!publisher.hasSubscribers())
    {
        std::this_thread::yield();
    }

    int ct = 1000;
    while (ct-- > 0)
    {
        publisher.publishCopyOf(Point{ct}).or_else([](auto) { std::cerr << "send failed\n"; });

        // if there is a waiting time, the subscriber can receive all 1000 messages.
        // if there is no waiting time, the subscriber cannot receive all 1000 messages.
        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::cout << "push end." << std::endl;
    iox::posix::waitForTerminationRequest();
    return 0;
}