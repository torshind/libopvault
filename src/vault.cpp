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

#include <sstream>
#include <sqlite3.h>

#include "dbg.h"
#include "vault.h"

using namespace std;

namespace OPVault {

Vault::Vault(const string &cloud_data_dir, const string &local_data_dir, const string &master_password) {
    if (FILE *file = fopen(string(local_data_dir + "opvault.db").c_str(), "r")) {
        fclose(file);
    } else {
        DBGMSG("create DB");
        create_db(cloud_data_dir);
    }

    get_profile();
    {
        Profile pro;
        pro.set_directory(cloud_data_dir);
        try {
            if (pro.read_updatedAt() > profile.updatedAt) {
                DBGMSG("update DB");
                remove(string(local_data_dir + "opvault.db").c_str());
                create_db(cloud_data_dir);
            }
        }
        catch (...) {
            DBGMSG("unable to read profile.js");
        }
    }
    profile.derive_keys(master_password);
    profile.get_overview_key();
    profile.get_master_key();
}

void Vault::get_profile() {
    sqlite3 *db;
    int rc;

    rc = sqlite3_open(DBFILE, &db);

    if(rc){
        ostringstream os;
        os << "libopvault: can't open database: " << sqlite3_errmsg(db) << " - error code: " << rc;
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    }

    sqlite3_stmt *stmt;
    if ((rc = sqlite3_prepare_v2(db, SQL_SELECT_PROFILE, -1, &stmt, nullptr)) != SQLITE_OK) {
        ostringstream os;
        os << "libopvault: SQL prepare error - error code: " << rc;
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    }


    else {
        if ((rc = sqlite3_step(stmt)) != SQLITE_ROW) {
            ostringstream os;
            os << "libopvault: profile table not present in DB - error code: " << rc;
            sqlite3_close(db);
            throw std::runtime_error(os.str());
        }
        profile.lastUpdatedBy = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        profile.updatedAt = sqlite3_column_int(stmt, 1);
        profile.profileName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        profile.salt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        profile.passwordHint = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        profile.masterKey = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        profile.iterations = (unsigned int) sqlite3_column_int(stmt, 6);
        profile.uuid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        profile.overviewKey = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        profile.createdAt = sqlite3_column_int(stmt, 9);

        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
}

void Vault::get_folders(vector<FolderEntry> &folders) const {
    sqlite3 *db;
    int rc;

    rc = sqlite3_open(DBFILE, &db);

    if(rc){
        ostringstream os;
        os << "libopvault: can't open database: " << sqlite3_errmsg(db) << " - error code: " << rc;
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    }

    sqlite3_stmt *stmt;
    if ((rc = sqlite3_prepare_v2(db, SQL_SELECT_FOLDERS, -1, &stmt, nullptr) != SQLITE_OK)) {
        ostringstream os;
        os << "libopvault: SQL prepare error - error code: " << rc;
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    }
    else {
        for (;;) {
            int rc = sqlite3_step(stmt);
            if (rc == SQLITE_DONE)
                break;
            if (rc != SQLITE_ROW) {
                ostringstream os;
                os << "libopvault: folders table not present in DB - error code: " << rc;
                sqlite3_close(db);
                throw std::runtime_error(os.str());
            }
            FolderEntry folder;
            folder.created = sqlite3_column_int(stmt, 0);
            folder.o = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            folder.tx = sqlite3_column_int(stmt, 2);
            folder.updated = sqlite3_column_int(stmt, 3);
            folder.uuid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            folders.push_back(folder);
        }
        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
}

void Vault::set_folders(const vector<FolderEntry> &folders) {
    Folder folder;
    folder.insert_all_entries(folders);
}

void Vault::get_items_query(const char query[], std::vector<BandEntry> &items) const {
    sqlite3 *db;
    int rc;

    rc = sqlite3_open(DBFILE, &db);

    if(rc){
        ostringstream os;
        os << "libopvault: can't open database: " << sqlite3_errmsg(db) << " - error code: " << rc;
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    }

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
        cout << "SQL prepare error" << endl;
    else {
        for (;;) {
            int rc = sqlite3_step(stmt);
            if (rc == SQLITE_DONE)
                break;
            if (rc != SQLITE_ROW) {
                ostringstream os;
                os << "libopvault: items table not present in DB - error code: " << rc;
                sqlite3_close(db);
                throw std::runtime_error(os.str());
            }
            BandEntry item;
            item.created = sqlite3_column_int(stmt, 0);
            item.o = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            item.tx = sqlite3_column_int(stmt, 2);
            item.updated = sqlite3_column_int(stmt, 3);
            item.uuid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            item.category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            item.d = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            item.fave = (unsigned long) sqlite3_column_int(stmt, 7);
            item.folder = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
            item.hmac = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
            item.k = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
            item.trashed = sqlite3_column_int(stmt, 11);
            items.push_back(item);
        }
        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
}

void Vault::create_db(const string &cloud_data_dir) {
    Profile pro;
    pro.set_directory(cloud_data_dir);
    try {
        pro.read();
    }
    catch (...) {
        throw;
    }
    pro.create_table();
    pro.insert_entry();


    Folder folders;
    try {
        folders.read();
    }
    catch (...) {
        throw;
    }
    folders.create_table();
    folders.insert_all_entries();

    Band band;
    try {
        band.read();
    }
    catch (...) {
        throw;
    }
    band.create_table();
    band.insert_all_entries();
}

void Vault::get_items(vector<BandEntry> &items) const {
    get_items_query(SQL_SELECT_ITEMS, items);
}

void Vault::set_items(const vector<BandEntry> &items) {
    Band band;
    band.insert_all_entries(items);
}

void Vault::get_items_folder(string folder, std::vector<BandEntry> &items) const {
    int sz = snprintf(nullptr, 0, SQL_SELECT_ITEMS_FOLDER, folder.c_str()) + 1;
    char *buf;
    buf = (char*) malloc((size_t) sz);
    snprintf(buf, (size_t) sz, SQL_SELECT_ITEMS_FOLDER, folder.c_str());

    get_items_query(buf, items);
    free(buf);
}

void Vault::get_items_category(string category, std::vector<BandEntry> &items) const {
    int sz = snprintf(nullptr, 0, SQL_SELECT_ITEMS_CATEGORY, category.c_str()) + 1;
    char *buf;
    buf = (char*) malloc((size_t) sz);
    snprintf(buf, (size_t) sz, SQL_SELECT_ITEMS_CATEGORY, category.c_str());

    get_items_query(buf, items);
    free(buf);
}

}
