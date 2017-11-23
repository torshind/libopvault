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

using namespace std;
using json = nlohmann::json;

namespace OPVault {

void Folder::read() {
    try {
        File::read("folders.js");
    }
    catch (...) {
        throw;
    }

    for(auto it = data.begin(); it !=data.end(); ++it) {
        FolderEntry folder = FolderEntry( (*it)["created"].is_number_integer() ? (*it)["created"].get<long>() : -1,
                                          (*it)["overview"].is_string() ? (*it)["overview"].get<string>() : "NULL",
                                          (*it)["tx"].is_number_integer() ? (*it)["tx"].get<long>() : -1,
                                          (*it)["updated"].is_number_integer() ? (*it)["updated"].get<long>() : -1,
                                          (*it)["uuid"].is_string() ? (*it)["uuid"].get<string>() : "NULL" );
        folders.push_back(folder);
    }
}

void Folder::create_table() {
    sql_exec(SQL_CREATE_FOLDERS);
}

void Folder::insert_entry(FolderEntry &folder) {
    int sz = snprintf(nullptr, 0, SQL_REPLACE_FOLDERS_ENTRY,
                      folder.created,
                      folder.o.c_str(),
                      folder.tx,
                      folder.updated,
                      folder.uuid.c_str()) + 1;
    char *buf;
    buf = (char*) malloc((size_t) sz);
    snprintf(buf, (size_t) sz, SQL_REPLACE_FOLDERS_ENTRY,
             folder.created,
             folder.o.c_str(),
             folder.tx,
             folder.updated,
             folder.uuid.c_str());

    DBGVAR(folder.uuid);

    sql_exec(buf);
    free(buf);
}

void Folder::insert_all_entries() {
    for(auto it=folders.begin(); it!=folders.end(); ++it) {
        insert_entry(*it);
    }
}

void Folder::insert_all_entries(std::vector<FolderEntry> folders) {
    for(auto it=folders.begin(); it!=folders.end(); ++it) {
        if (it->updateState) {
            insert_entry(*it);
        }
    }
}

}
