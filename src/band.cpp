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
#include "banditem.h"
#include "vault.h"

#include "band.h"

using json = nlohmann::json;

namespace OPVault {

void Band::read() {
    nlohmann::json j;
    int exept_count = 0;

    for (int index=0; index<BAND_NUM; ++index) {
        try {
            File::read(std::string("band_") + BAND_INDEXES[index] + std::string(".js"), j);
        }
        catch (...) {
            exept_count++;
            if (exept_count == BAND_NUM)
            {
                throw;
            }
            continue;
        }
        for(auto it=j.begin(); it!=j.end(); ++it) {
            insert_json(*it);
        }
    }
}

void Band::create_table() {
    sql_exec(SQL_CREATE_ITEMS);
}

void Band::insert_item(BaseItem* base_item) {
    BandItem* item = static_cast<BandItem*>(base_item);
    int sz = snprintf(nullptr, 0, SQL_REPLACE_ITEM,
                      item->created,
                      item->o.c_str(),
                      item->tx,
                      item->updated,
                      item->uuid.c_str(),
                      item->category.c_str(),
                      item->d.c_str(),
                      item->fave,
                      item->folder.c_str(),
                      item->hmac.c_str(),
                      item->k.c_str(),
                      item->trashed) + 1;
    char *buf;
    buf = (char*) malloc((size_t) sz);
    snprintf(buf, (size_t) sz, SQL_REPLACE_ITEM,
             item->created,
             item->o.c_str(),
             item->tx,
             item->updated,
             item->uuid.c_str(),
             item->category.c_str(),
             item->d.c_str(),
             item->fave,
             item->folder.c_str(),
             item->hmac.c_str(),
             item->k.c_str(),
             item->trashed);

    DBGVAR(item->uuid);

    sql_exec(buf);
    free(buf);
}

void Band::insert_items(std::vector<BandItem> &items) {
    for(auto it=items.begin(); it!=items.end(); ++it) {
        if (it->updateState) {
            it->generate_hmac();
            insert_item(&*it);
            it->updateState = false;
        } else {
            insert_item(&*it);
        }
    }
}

void Band::sync(std::vector<BandItem> &items) {
    nlohmann::json j;
    std::unordered_map<std::string, BandItem*> local_map;

    for (auto it=items.begin(); it!=items.end(); ++it) {
        // Create a map with key uuid for local items
        local_map.insert({it->uuid, &*it});
    }

    int exept_count = 0;
    for (int index=0; index<BAND_NUM; ++index) {
        try {
            File::read(std::string("band_") + BAND_INDEXES[index] + std::string(".js"), j);
        }
        catch (...) {
            exept_count++;
            if (exept_count == BAND_NUM)
            {
                throw;
            }
            continue;
        }
        for(auto it=j.begin(); it!=j.end(); ++it) {
            // Find uuid in local map
            std::string uuid = (*it)["uuid"].is_string() ? (*it)["uuid"].get<std::string>() : "NULL";

            auto found = local_map.find(uuid);
            if (found != local_map.end()) {
                // Get remote tx
                long tx;
                try {
                    tx = (*it)["tx"].is_number_integer() ? (*it)["tx"].get<long>() : -1;
                }
                catch (...) {
                    throw;
                }
                DBGVAR(tx);

                // Check for local changes: local.updated > local.tx
                DBGVAR(found->second->updated);
                DBGVAR(found->second->tx);
                if (found->second->updated > found->second->tx) {
                    if (tx > found->second->tx) {
                        DBGMSG("merge local & remote changes");
                    } else {
                        DBGMSG("sync remote with local item");
                        // Remove from map
                        local_map.erase(found->first);
                    }
                } else {
                    // Check for remote changes: remote.tx > local.tx
                    if (tx > found->second->tx) {
                        DBGMSG("sync local with remote item");
                        insert_json(*it);

                        // Remove from map
                        local_map.erase(found->first);
                    }
                }
            } else {
                // New remote item: insert in db
                DBGMSG("new remote item");
                insert_json(*it);
            }
        }
    }
    // New local items: sync new elements still in the map
    if (!local_map.empty()) {
        DBGMSG("new local item");
    }
}

BaseItem* Band::json2item(json &j) {
  return new BandItem(j["created"].is_number_integer() ? j["created"].get<long>() : -1,
                      j["o"].is_string() ? j["o"].get<std::string>() : "NULL",
                      j["tx"].is_number_integer() ? j["tx"].get<long>() : -1,
                      j["updated"].is_number_integer() ? j["updated"].get<long>() : -1,
                      j["uuid"].is_string() ? j["uuid"].get<std::string>() : "NULL",
                      j["category"].is_string() ? j["category"].get<std::string>() : "NULL",
                      j["d"].is_string() ? j["d"].get<std::string>() : "NULL",
                      j["fave"].is_number_integer() ? j["fave"].get<unsigned long>() : 0,
                      j["folder"].is_string() ? j["folder"].get<std::string>() : "NULL",
                      j["hmac"].is_string() ? j["hmac"].get<std::string>() : "NULL",
                      j["k"].is_string() ? j["k"].get<std::string>() : "NULL",
                      j["trashed"].is_boolean() ? j["trashed"].get<int>() : -1);
}

}
