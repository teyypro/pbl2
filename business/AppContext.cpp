#include "AppContext.h"

AppContext::AppContext(const string& dataFolder)
    : db(dataFolder), auth(db), doc(db), reg(db), dash(db), queue(db, dash)
{
}