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

#include "dbg.h"
#include "vault.h"

#include "profile.h"

using json = nlohmann::json;

namespace OPVault {

void Profile::read() {
    nlohmann::json j;

    try {
        File::read("profile.js", j);
    }
    catch (...) {
        throw;
    }
    insert_json(j);
}

int Profile::read_updatedAt() {
    nlohmann::json j;

    try {
        File::read("profile.js", j);
    }
    catch (...) {
        throw;
    }
    return j["updatedAt"];
}

void Profile::create_table() {
    sql_exec(SQL_CREATE_PROFILE);
}

void Profile::insert_item(BaseItem* base_item) {
    ProfileItem* profile = static_cast<ProfileItem*>(base_item);
    int sz = snprintf(nullptr, 0, SQL_INSERT_PROFILE_ITEM,
                      profile->lastUpdatedBy.c_str(),
                      profile->updatedAt,
                      profile->profileName.c_str(),
                      profile->salt.c_str(),
                      profile->passwordHint.c_str(),
                      profile->masterKey.c_str(),
                      profile->iterations,
                      profile->uuid.c_str(),
                      profile->overviewKey.c_str(),
                      profile->createdAt) + 1;
    char *buf;
    buf = (char*) malloc((size_t) sz);
    snprintf(buf, (size_t) sz, SQL_INSERT_PROFILE_ITEM,
             profile->lastUpdatedBy.c_str(),
             profile->updatedAt,
             profile->profileName.c_str(),
             profile->salt.c_str(),
             profile->passwordHint.c_str(),
             profile->masterKey.c_str(),
             profile->iterations,
             profile->uuid.c_str(),
             profile->overviewKey.c_str(),
             profile->createdAt);

    DBGVAR(profile->uuid);

    sql_exec(buf);
    free(buf);
}

BaseItem* Profile::json2item(nlohmann::json &j) {
    return new ProfileItem(j["lastUpdatedBy"].is_string() ? j["lastUpdatedBy"].get<std::string>() : "NULL",
                           j["updatedAt"].is_number_integer() ? j["updatedAt"].get<long>() : -1,
                           j["profileName"].is_string() ? j["profileName"].get<std::string>() : "NULL",
                           j["salt"].is_string() ? j["salt"].get<std::string>() : "NULL",
                           j["passwordHint"].is_string() ? j["passwordHint"].get<std::string>() : "NULL",
                           j["masterKey"].is_string() ? j["masterKey"].get<std::string>() : "NULL",
                           j["iterations"].is_number_integer() ? j["iterations"].get<unsigned int>() : 0,
                           j["uuid"].is_string() ? j["uuid"].get<std::string>() : "NULL",
                           j["overviewKey"].is_string() ? j["overviewKey"].get<std::string>() : "NULL",
                           j["createdAt"].is_number_integer() ? j["createdAt"].get<long>() : -1);
}

}
