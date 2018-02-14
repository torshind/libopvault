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
    setup_filenames();

    try {
        File::read(filenames);
    } catch (...) {
        throw;
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
    for(auto &item : items) {
        if (item.updateState) {
            item.generate_hmac();
            insert_item(&item);
            item.updateState = false;
        } else {
            insert_item(&item);
        }
    }
}

void Band::sync(std::vector<BandItem> &items) {
    std::unordered_map<std::string, UserItem*> local_map;

    for (auto &item : items) {
        // Create a map with key uuid for local items
        local_map.insert({item.uuid, &item});
    }

    setup_filenames();

    try {
        File::sync(filenames, local_map);
    } catch (...) {
        throw;
    }

    // New local items: sync new elements still in the map
    if (!local_map.empty()) {
        for (auto const &item : local_map) {
            DBGMSG("new local item");
            append(std::string("band_") + item.second->get_uuid()[0] + std::string(".js"), item.second);
        }
    }
}

BaseItem* Band::json2item(json &j) {
  return new BandItem(
      j["created"].is_number_integer() ? j["created"].get<long>() : -1,
      j["o"].is_string() ? j["o"].get<std::string>() : "",
      j["tx"].is_number_integer() ? j["tx"].get<long>() : -1,
      j["updated"].is_number_integer() ? j["updated"].get<long>() : -1,
      j["uuid"].is_string() ? j["uuid"].get<std::string>() : "",
      j["category"].is_string() ? j["category"].get<std::string>() : "",
      j["d"].is_string() ? j["d"].get<std::string>() : "",
      j["fave"].is_number_integer() ? j["fave"].get<long>() : -1,
      j["folder"].is_string() ? j["folder"].get<std::string>() : "",
      j["hmac"].is_string() ? j["hmac"].get<std::string>() : "",
      j["k"].is_string() ? j["k"].get<std::string>() : "",
      j["trashed"].is_boolean() ? j["trashed"].get<int>() : j["trashed"].is_string() ? j["trashed"].get<int>() : -1);
}

void Band::setup_filenames() {
    if (filenames.empty()) {
        for (int index = 0; index < BAND_NUM; ++index) {
            filenames.push_back(std::string("band_") + BAND_INDEXES[index] + std::string(".js"));
        }
    }
}

void Band::update_tx(BaseItem* base_item) {
    BandItem* item = static_cast<BandItem*>(base_item);
    // update tx and hmac in db
    item->tx = time(nullptr);
    item->generate_hmac();
    insert_item(item);
}

}
