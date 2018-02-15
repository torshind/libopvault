/*
libopvault

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2017 Marcello V. Mansueto

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "json.hpp"

#include "baseitem.h"

namespace OPVault {

class UserItem : public BaseItem {
    friend class File;

protected:
    UserItem() {
        tx = 0;
    }

    UserItem(long _created,
             std::string _o,
             long _tx,
             long _updated,
             std::string _uuid) :
        created(_created),
        o(_o),
        tx(_tx),
        updated(_updated),
        uuid(_uuid)
    {}

    virtual ~UserItem() {}

    virtual void init();
    void setup_update();
    virtual void to_json(nlohmann::json &j) = 0;

public:
    std::string& get_overview() { return o; }
    std::string& get_uuid() { return uuid; }

    void set_overview(const std::string &_o);

    void decrypt_overview(std::string &overview);

protected:
    long created;
    std::string o;
    long tx;
    long updated;
    std::string uuid;

    bool updateState;
};

}
