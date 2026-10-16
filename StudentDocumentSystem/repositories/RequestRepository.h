#pragma once
#include <string>
#include "../models/Request.h"
#include "../templates/MyList.h"
#include "IRequestRepository.h"

using namespace std;

class RequestRepository : public IRequestRepository {
private:
    string path;
public:
    explicit RequestRepository(const string& directory);
    MyList<Request> Load() const override;
    void Save(const MyList<Request>& items) const override;
};
