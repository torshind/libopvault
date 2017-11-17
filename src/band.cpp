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
#include "bandentry.h"
#include "vault.h"

#include "band.h"

using namespace std;
using json = nlohmann::json;

namespace OPVault {

void Band::read() {
    int exept_count = 0;
    for (char index=0; index<BAND_NUM; ++index) {
        try {
            File::read(string("band_") + BAND_INDEXES[index] + string(".js"));
        }
        catch (...) {
            exept_count++;
            if (exept_count == BAND_NUM)
            {
                throw;
                return;
            }
            continue;
        }
        for(json::iterator it=data.begin(); it!=data.end(); ++it) {
            BandEntry item;
            try {
                item = BandEntry( (*it)["created"],
                                  (*it)["o"].is_string() ? (*it)["o"].get<string>() : "NULL",
                                  (*it)["tx"],
                                  (*it)["updated"],
                                  (*it)["uuid"].is_string() ? (*it)["uuid"].get<string>() : "NULL",
                                  (*it)["category"].is_string() ? (*it)["category"].get<string>() : "NULL",
                                  (*it)["d"].is_string() ? (*it)["d"].get<string>() : "NULL",
                                  (*it)["folder"].is_string() ? (*it)["folder"].get<string>() : "NULL",
                                  (*it)["hmac"].is_string() ? (*it)["hmac"].get<string>() : "NULL",
                                  (*it)["k"].is_string() ? (*it)["k"].get<string>() : "NULL" );
            }
            catch (...) {
                throw;
            }
            items.push_back(item);
        }
    }
}

void Band::create_table() {
    Vault::sql_exec(SQL_CREATE_ITEMS);
}

void Band::insert_entry(BandEntry &item) {
    int sz = snprintf(nullptr, 0, SQL_REPLACE_ITEMS_ENTRY,
                      item.created,
                      item.o.c_str(),
                      item.tx,
                      item.updated,
                      item.uuid.c_str(),
                      item.category.c_str(),
                      item.d.c_str(),
                      item.folder.c_str(),
                      item.hmac.c_str(),
                      item.k.c_str()) + 1;
    char *buf;
    buf = (char*) malloc(sz);
    snprintf(buf, sz, SQL_REPLACE_ITEMS_ENTRY,
             item.created,
             item.o.c_str(),
             item.tx,
             item.updated,
             item.uuid.c_str(),
             item.category.c_str(),
             item.d.c_str(),
             item.folder.c_str(),
             item.hmac.c_str(),
             item.k.c_str());

    DBGVAR(item.uuid);

    Vault::sql_exec(buf);
    free(buf);
}

void Band::insert_all_entries() {
    for(vector<BandEntry>::iterator it=items.begin(); it!=items.end(); ++it) {
        insert_entry(*it);
    }
}

void Band::insert_all_entries(std::vector<BandEntry> items) {
    for(vector<BandEntry>::iterator it=items.begin(); it!=items.end(); ++it) {
        if (it->k.empty()) {
            it->init();
            it->updateState.update_d = true;
            it->updateState.update_o = true;
        }
        if (it->updateState.update_d) {
            it->encrypt_data();
        }
        if (it->updateState.update_o) {
            it->encrypt_overview();
        }
        if (it->updateState.update_d || it->updateState.update_o || it->updateState.update_plain) {
            it->generate_hmac();
            insert_entry(*it);
        }
    }
}

}
