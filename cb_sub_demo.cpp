#include <iceoryx/v/iceoryx_posh/popo/subscriber.hpp>
#include <iceoryx_dust/posix_wrapper/signal_watcher.hpp>
#include <iceoryx_posh/popo/listener.hpp>
#include <iostream>

#include "type.hpp"

int counter = 0;
int64_t dt_max = 0;
int64_t dt_min = 999999999;
void onSampleReceivedCallback(iox::popo::Subscriber<Test> *subscriber)
{
    // std::cout << "thread id: " << std::this_thread::get_id() << std::endl;
    while (subscriber->hasData())
    {
        counter++;
        subscriber->take().and_then([](auto &sample) {
            int64_t server_recv_count = std::chrono::duration_cast<std::chrono::microseconds>(
                                            std::chrono::system_clock::now().time_since_epoch())
                                            .count();
            int64_t dt = server_recv_count - sample->client_req_dt;
            dt_max = std::max(dt_max, dt);
            dt_min = std::min(dt_min, dt);

            if (counter % 1000 == 0)
            {
                std::cout << "received x: " << sample->x << ", counter:" << counter << std::endl;
                std::cout << "dt:" << dt << ", max:" << dt_max << ", min:" << dt_min << std::endl;
            }
        });
    }
}

int main()
{
    iox::runtime::PoshRuntime::initRuntime("test_cb_sub");

    iox::popo::SubscriberOptions options;
    options.nodeName = "Sub_Node_With_Options";
    options.queueFullPolicy = iox::popo::QueueFullPolicy::BLOCK_PRODUCER;  // 必要
    iox::popo::Subscriber<Test> subscriber({"aa", "bb", "cc"}, options);

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
