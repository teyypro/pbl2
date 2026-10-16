#pragma once
#include "../models/Request.h"
#include "../templates/MyList.h"
class IRequestRepository {
public:
    virtual ~IRequestRepository() = default;
    virtual MyList<Request> Load() const = 0;
    virtual void Save(const MyList<Request>& requests) const = 0;
};
