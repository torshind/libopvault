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

using namespace std;
using json = nlohmann::json;

namespace OPVault {

void Profile::read() {
    try {
        File::read("profile.js");
    }
    catch (...) {
        throw;
    }
    profile = ProfileEntry( data["lastUpdatedBy"].is_string() ? data["lastUpdatedBy"].get<string>() : "NULL",
                            data["updatedAt"].is_number_integer() ? data["updatedAt"].get<long>() : -1,
                            data["profileName"].is_string() ? data["profileName"].get<string>() : "NULL",
                            data["salt"].is_string() ? data["salt"].get<string>() : "NULL",
                            data["passwordHint"].is_string() ? data["passwordHint"].get<string>() : "NULL",
                            data["masterKey"].is_string() ? data["masterKey"].get<string>() : "NULL",
                            data["iterations"].is_number_integer() ? data["iterations"].get<unsigned int>() : 0,
                            data["uuid"].is_string() ? data["uuid"].get<string>() : "NULL",
                            data["overviewKey"].is_string() ? data["overviewKey"].get<string>() : "NULL",
                            data["createdAt"].is_number_integer() ? data["createdAt"].get<long>() : -1 );
}

int Profile::read_updatedAt() {
    try {
        File::read("profile.js");
    }
    catch (...) {
        throw;
    }
    return data["updatedAt"];
}

void Profile::create_table() {
    Vault::sql_exec(SQL_CREATE_PROFILE);
}

void Profile::insert_entry() {
    int sz = snprintf(nullptr, 0, SQL_INSERT_PROFILE_ENTRY,
                      profile.lastUpdatedBy.c_str(),
                      profile.updatedAt,
                      profile.profileName.c_str(),
                      profile.salt.c_str(),
                      profile.passwordHint.c_str(),
                      profile.masterKey.c_str(),
                      profile.iterations,
                      profile.uuid.c_str(),
                      profile.overviewKey.c_str(),
                      profile.createdAt) + 1;
    char *buf;
    buf = (char*) malloc((size_t) sz);
    snprintf(buf, (size_t) sz, SQL_INSERT_PROFILE_ENTRY,
             profile.lastUpdatedBy.c_str(),
             profile.updatedAt,
             profile.profileName.c_str(),
             profile.salt.c_str(),
             profile.passwordHint.c_str(),
             profile.masterKey.c_str(),
             profile.iterations,
             profile.uuid.c_str(),
             profile.overviewKey.c_str(),
             profile.createdAt);

    DBGVAR(profile.uuid);

    Vault::sql_exec(buf);
    free(buf);
}

}
