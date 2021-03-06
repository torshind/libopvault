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
#include "const.h"
#include "vault.h"

#include "folder.h"

namespace OPVault {

void Folder::read() {
    try {
        File::read("folders.js");
    } catch (...) {
        throw;
    }
}

void Folder::create_table() {
    sql_exec(SQL_CREATE_FOLDERS);
}

void Folder::insert_item(BaseItem* base_item) {
    FolderItem* folder = static_cast<FolderItem*>(base_item);
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
    if ((rc = sqlite3_prepare_v2(db, SQL_REPLACE_FOLDER, -1, &stmt, nullptr) != SQLITE_OK)) {
        std::ostringstream os;
        os << "libopvault: SQL prepare error - error code: " << rc;
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    } else {
        sqlite3_bind_int64(stmt, 1, folder->created);
        sqlite3_bind_text(stmt, 2, folder->o.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 3, folder->tx);
        sqlite3_bind_int64(stmt, 4, folder->updated);
        sqlite3_bind_text(stmt, 5, folder->uuid.c_str(), -1, SQLITE_STATIC);

        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::ostringstream os;
            os << "libopvault: error replacing data in Folders table - error code: " << rc;
            sqlite3_close(db);
            throw std::runtime_error(os.str());
        }

        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
}

void Folder::insert_folders(std::vector<FolderItem> &folders) {
    for(auto &folder : folders) {
        if (folder.updateState) {
            insert_item(&folder);
            folder.updateState = false;
        }
    }
}

void Folder::sync(std::vector<FolderItem> &folders) {
    std::unordered_map<std::string, UserItem*> local_map;

    for (auto &folder : folders) {
        // Create a map with key uuid for local items
        local_map.insert({folder.uuid, &folder});
    }

    try {
        File::sync("folders.js", local_map);
    }
    catch (...) {
        throw;
    }

    // New local items: sync new elements still in the map
    if (!local_map.empty()) {
        for (auto const &folder : local_map) {
            append("folders.js", folder.second);
        }
    }
}

BaseItem* Folder::json2item(nlohmann::json &j) {
    return new FolderItem(j["created"].is_number_integer() ? j["created"].get<long>() : -1,
                          j["overview"].is_string() ? j["overview"].get<std::string>() : "",
                          j["tx"].is_number_integer() ? j["tx"].get<long>() : -1,
                          j["updated"].is_number_integer() ? j["updated"].get<long>() : -1,
                          j["uuid"].is_string() ? j["uuid"].get<std::string>() : "");
}

void Folder::update_tx(BaseItem* base_item) {
    FolderItem* folder = static_cast<FolderItem*>(base_item);
    // update tx in db
    folder->tx = time(nullptr);
    insert_item(folder);
}

}
