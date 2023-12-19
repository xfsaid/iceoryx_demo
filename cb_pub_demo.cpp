#include <chrono>
#include <iceoryx/v/iceoryx_posh/popo/publisher.hpp>
#include <iceoryx_dust/posix_wrapper/signal_watcher.hpp>
#include <iostream>
#include <thread>

#include "type.hpp"

int main()
{
    iox::runtime::PoshRuntime::initRuntime("test_cb_pub");

    iox::popo::PublisherOptions options;
    options.nodeName = "Pub_Node_With_Options";
    options.subscriberTooSlowPolicy = iox::popo::ConsumerTooSlowPolicy::WAIT_FOR_CONSUMER;  // 必要
    iox::popo::Publisher<Test> publisher({"aa", "bb", "cc"}, options);

    {  // 非必要等待，会导致cpu占用高
        publisher.offer();
        while (!publisher.hasSubscribers())
        {
            std::this_thread::yield();
        }
    }

    int64_t count_start = std::chrono::duration_cast<std::chrono::microseconds>(
                              std::chrono::system_clock::now().time_since_epoch())
                              .count();

    int ct = 10 * 10000;
    while (ct-- > 0)
    {
        Test test{ct};
        test.client_req_dt = std::chrono::duration_cast<std::chrono::microseconds>(
                                 std::chrono::system_clock::now().time_since_epoch())
                                 .count();
        publisher.publishCopyOf(test).or_else([](auto) { std::cerr << "send failed\n"; });

        // if there is a waiting time, the subscriber can receive all 1000 messages.
        // if there is no waiting time, the subscriber cannot receive all 1000 messages.
        // std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    int64_t count_dt = std::chrono::duration_cast<std::chrono::microseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count() -
                       count_start;

    std::cout << "push end. count_dt:" << count_dt / 1000 << "(ms)" << std::endl;
    // iox::posix::waitForTerminationRequest();
    return 0;
}