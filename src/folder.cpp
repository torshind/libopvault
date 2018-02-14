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
#include "const.h"
#include "vault.h"

#include "folder.h"

using json = nlohmann::json;

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
    int sz = snprintf(nullptr, 0, SQL_REPLACE_FOLDER,
                      folder->created,
                      folder->o.c_str(),
                      folder->tx,
                      folder->updated,
                      folder->uuid.c_str()) + 1;
    char *buf;
    buf = (char*) malloc((size_t) sz);
    snprintf(buf, (size_t) sz, SQL_REPLACE_FOLDER,
             folder->created,
             folder->o.c_str(),
             folder->tx,
             folder->updated,
             folder->uuid.c_str());

    DBGVAR(folder->uuid);

    sql_exec(buf);
    free(buf);
}

void Folder::insert_folders(std::vector<FolderItem> &folders) {
    for(auto it : folders) {
        if (it.updateState) {
            insert_item(&it);
            it.updateState = false;
        }
    }
}

void Folder::sync(std::vector<FolderItem> &folders) {
    std::unordered_map<std::string, UserItem*> local_map;

    for (auto it=folders.begin(); it!=folders.end(); ++it) {
        // Create a map with key uuid for local items
        local_map.insert({it->uuid, &*it});
    }

    try {
        File::sync("folders.js", local_map);
    }
    catch (...) {
        throw;
    }

    // New local items: sync new elements still in the map
    if (!local_map.empty()) {
        for (auto it : local_map) {
            append("folders.js", it.second);
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

void Folder::folder2json(FolderItem *folder, nlohmann::json &j) {
    json j_item;
    j_item["created"]  = folder->created;
    j_item["o"]        = folder->o;
    j_item["tx"]       = folder->tx;
    j_item["updated"]  = folder->updated;
    j_item["uuid"]     = folder->uuid;

    j[folder->uuid] = j_item;
}

void Folder::update_tx(BaseItem* base_item) {
    FolderItem* folder = static_cast<FolderItem*>(base_item);
    // update tx in db
    folder->tx = time(nullptr);
    insert_item(folder);
}

}
