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

#include <sqlite3.h>

#include "dbg.h"
#include "vault.h"

#include "profile.h"

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
    sqlite3 *db;
    int rc;

    rc = sqlite3_open(DBFILE, &db);

    if(rc){
        std::ostringstream os;
        os << "libopvault: can't open database: " << sqlite3_errmsg(db) << " - error code: " << rc;
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    }

    sqlite3_stmt *stmt;
    if ((rc = sqlite3_prepare_v2(db, SQL_INSERT_PROFILE_ITEM, -1, &stmt, nullptr) != SQLITE_OK)) {
        std::ostringstream os;
        os << "libopvault: SQL prepare error - error code: " << rc;
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    } else {
        sqlite3_bind_text(stmt, 1, profile->lastUpdatedBy.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 2, profile->updatedAt);
        sqlite3_bind_text(stmt, 3, profile->profileName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, profile->salt.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, profile->passwordHint.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, profile->masterKey.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 7, profile->iterations);
        sqlite3_bind_text(stmt, 8, profile->uuid.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 9, profile->overviewKey.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 10, profile->createdAt);

        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::ostringstream os;
            os << "libopvault: error inserting data in Profile table - error code: " << rc;
            sqlite3_close(db);
            throw std::runtime_error(os.str());
        }

        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
}

BaseItem* Profile::json2item(nlohmann::json &j) {
    return new ProfileItem(j["lastUpdatedBy"].is_string() ? j["lastUpdatedBy"].get<std::string>() : "",
                           j["updatedAt"].is_number_integer() ? j["updatedAt"].get<long>() : -1,
                           j["profileName"].is_string() ? j["profileName"].get<std::string>() : "",
                           j["salt"].is_string() ? j["salt"].get<std::string>() : "",
                           j["passwordHint"].is_string() ? j["passwordHint"].get<std::string>() : "",
                           j["masterKey"].is_string() ? j["masterKey"].get<std::string>() : "",
                           j["iterations"].is_number_integer() ? j["iterations"].get<unsigned int>() : 0,
                           j["uuid"].is_string() ? j["uuid"].get<std::string>() : "",
                           j["overviewKey"].is_string() ? j["overviewKey"].get<std::string>() : "",
                           j["createdAt"].is_number_integer() ? j["createdAt"].get<long>() : -1);
}

void Profile::update_tx(BaseItem* base_item) {
    // PROFILE SYNC IS NOT SUPPORTED
}

}
