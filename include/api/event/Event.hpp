#pragma once
#include "Macros.hpp"

class Event {
protected:
    bool mBlockPass;

public:
    KobeBryant_NDAPI Event();

    KobeBryant_NDAPI virtual ~Event();

    KobeBryant_API void block_pass();

    KobeBryant_NDAPI bool isPassingBlocked();
};

class CancellableEvent : public Event {
protected:
    bool mCanceled;

public:
    KobeBryant_NDAPI CancellableEvent();

    KobeBryant_API void cancel();

    KobeBryant_NDAPI bool isCancelled();
};