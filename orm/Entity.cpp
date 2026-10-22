#include "Entity.h"

string Entity::getKey() const
{
    Row r = toRow();
    return r.isEmpty() ? string("") : r[0];
}