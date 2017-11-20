#ifndef USERENTRY_H
#define USERENTRY_H

#include "baseentry.h"

namespace OPVault {

class UserEntry : protected BaseEntry {
protected:
    UserEntry() {}

    UserEntry(int _created,
              std::string _o,
              int _tx,
              int _updated,
              std::string _uuid) :
        created(_created),
        o(_o),
        tx(_tx),
        updated(_updated),
        uuid(_uuid)
    {}

public:
    std::string get_overview() { return o; }
    std::string get_uuid() { return uuid; }

    void set_overview(std::string _o);

    void decrypt_overview(std::string& overview);

protected:
    int created;
    std::string o;
    int tx;
    int updated;
    std::string uuid;

    bool updateState;
};

}

#endif // USERENTRY_H