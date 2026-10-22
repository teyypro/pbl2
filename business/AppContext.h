#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include "../orm/DbContext.h"
#include "AuthService.h"
#include "DocumentService.h"
#include "RegistrationService.h"
#include "DashboardService.h"
#include "QueueService.h"

// Gom DbContext + các service để truyền cho tầng Presentation
// (thứ tự khai báo = thứ tự khởi tạo)
class AppContext
{
public:
    DbContext db;
    AuthService auth;
    DocumentService doc;
    RegistrationService reg;
    DashboardService dash;
    QueueService queue;

    AppContext(const string& dataFolder);
};

#endif