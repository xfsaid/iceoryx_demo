#include <atomic>
#include <iceoryx/v/iceoryx_posh/popo/subscriber.hpp>
#include <iceoryx_dust/posix_wrapper/signal_watcher.hpp>
#include <iceoryx_posh/popo/listener.hpp>
#include <iostream>

struct Point
{
    int x;
};

std::atomic<int> counter = 0;
void onSampleReceivedCallback(iox::popo::Subscriber<Point> *subscriber)
{
    counter++;
    subscriber->take().and_then([subscriber](auto &sample) {
        std::cout << "received x: " << sample->x << ", counter:" << counter << std::endl;
    });
}

int main()
{
    iox::runtime::PoshRuntime::initRuntime("test_cb_sub");

    iox::popo::SubscriberOptions options;
    options.nodeName = "Sub_Node_With_Options";
    options.queueFullPolicy = iox::popo::QueueFullPolicy::BLOCK_PRODUCER;
    iox::popo::Subscriber<Point> subscriber({"aa", "bb", "cc"}, options);

    iox::popo::Listener listener;
    listener
        .attachEvent(subscriber, iox::popo::SubscriberEvent::DATA_RECEIVED,
                     iox::popo::createNotificationCallback(onSampleReceivedCallback))
        .or_else([](auto) {
            std::cerr << "unable to attach subscriber" << std::endl;
            std::exit(EXIT_FAILURE);
        });

    iox::posix::waitForTerminationRequest();
    return (EXIT_SUCCESS);
}
