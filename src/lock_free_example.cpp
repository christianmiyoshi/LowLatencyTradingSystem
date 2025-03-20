#include <chrono>
#include "thread_utils.h"

#include "lock_free_queue.h"

struct MyStruct {
    int d_[3];
};

using namespace Common;

auto consumeFunction(LFQueue<MyStruct> *lfq) {
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(5s);

    while(lfq->size() > 0) {
        auto elem = lfq->getNextToRead();
        lfq->updateReadIndex();

        std::cout << "Consumed: " << elem->d_[0] << ", " << elem->d_[1] << ", " << elem->d_[2] << " LFQ-size" << lfq->size() << std::endl;
        std::this_thread::sleep_for(1s);
    }

    std::cout << "ConsumeFunction exiting" << std::endl;
}

int main(int, char**){

    LFQueue<MyStruct> lfq(20);

    auto ct = createAndStartThread(-1, "consumeFunction", consumeFunction, &lfq);

    for (auto i=0; i<50; ++i) {
        const MyStruct d{i, i*10, i*100};
        *(lfq.getNextToWriteTo()) = d;
        lfq.updateWriteIndex();

        std::cout << "Produced: " << d.d_[0] << ", " << d.d_[1] << ", " << d.d_[2] << " LFQ-size" << lfq.size() << std::endl;
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);
    }

    ct->join();
    std::cout << "Main thread done" << std::endl;

    return 0;
}