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

namespace OPVault {

Vault::Vault(const std::string &cloud_data_dir, const std::string &local_data_dir, const std::string &master_password) {
    if (FILE *file = fopen(std::string(local_data_dir + "opvault.db").c_str(), "r")) {
        fclose(file);
    } else {
        DBGMSG("create DB");
        create_db(cloud_data_dir);
        get_profile();
        setup_profile(master_password);
        return;
    }

    Profile pro;

    get_profile();
    pro.set_directory(cloud_data_dir);
    try {
        if (pro.read_updatedAt() > profile.updatedAt) {
            DBGMSG("Profile updated -> refreshing local DB");
            remove(std::string(local_data_dir + "opvault.db").c_str());
            create_db(cloud_data_dir);
        } else {
            setup_profile(master_password);
            sync();
        }
    }
    catch (...) {
        DBGMSG("unable to read profile.js");
    }
}

void Vault::get_profile() {
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
    if ((rc = sqlite3_prepare_v2(db, SQL_SELECT_PROFILE, -1, &stmt, nullptr)) != SQLITE_OK) {
        std::ostringstream os;
        os << "libopvault: SQL prepare error - error code: " << rc;
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    } else {
        if ((rc = sqlite3_step(stmt)) != SQLITE_ROW) {
            std::ostringstream os;
            os << "libopvault: profile table not present in DB - error code: " << rc;
            sqlite3_close(db);
            throw std::runtime_error(os.str());
        }
        profile.lastUpdatedBy = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        profile.updatedAt = sqlite3_column_int64(stmt, 1);
        profile.profileName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        profile.salt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        profile.passwordHint = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        profile.masterKey = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        profile.iterations = (unsigned int) sqlite3_column_int(stmt, 6);
        profile.uuid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        profile.overviewKey = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        profile.createdAt = sqlite3_column_int64(stmt, 9);

        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
}

void Vault::setup_profile(const std::string &master_password) {
    profile.derive_keys(master_password);
    profile.get_overview_key();
    profile.get_master_key();
}

void Vault::get_folders(std::vector<FolderItem> &folders) const {
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
    if ((rc = sqlite3_prepare_v2(db, SQL_SELECT_FOLDERS, -1, &stmt, nullptr) != SQLITE_OK)) {
        std::ostringstream os;
        os << "libopvault: SQL prepare error - error code: " << rc;
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    } else {
        for (;;) {
            int rc = sqlite3_step(stmt);
            if (rc == SQLITE_DONE)
                break;
            if (rc != SQLITE_ROW) {
                std::ostringstream os;
                os << "libopvault: folders table not present in DB - error code: " << rc;
                sqlite3_close(db);
                throw std::runtime_error(os.str());
            }
            FolderItem folder;
            folder.created = sqlite3_column_int64(stmt, 0);
            folder.o = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            folder.tx = sqlite3_column_int64(stmt, 2);
            folder.updated = sqlite3_column_int64(stmt, 3);
            folder.uuid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            folders.push_back(folder);
        }
        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
}

void Vault::insert_folders(std::vector<FolderItem> &folders) {
    Folder folder;
    folder.insert_folders(folders);
}

void Vault::get_items_query(const char query[], std::vector<BandItem> &items) const {
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
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cout << "SQL prepare error" << std::endl;
    } else {
        for (;;) {
            int rc = sqlite3_step(stmt);
            if (rc == SQLITE_DONE)
                break;
            if (rc != SQLITE_ROW) {
                std::ostringstream os;
                os << "libopvault: items table not present in DB - error code: " << rc;
                sqlite3_close(db);
                throw std::runtime_error(os.str());
            }
            BandItem item;
            item.created = sqlite3_column_int64(stmt, 0);
            item.o = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            item.tx = sqlite3_column_int64(stmt, 2);
            item.updated = sqlite3_column_int64(stmt, 3);
            item.uuid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            item.category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            item.d = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            item.fave = sqlite3_column_int64(stmt, 7);
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

void Vault::create_db(const std::string &cloud_data_dir) {
    Profile pro;
    pro.set_directory(cloud_data_dir);
    pro.create_table();
    try {
        pro.read();
    }
    catch (...) {
        throw;
    }

    Folder folder;
    folder.create_table();
    try {
        folder.read();
    }
    catch (...) {
        throw;
    }

    Band band;
    band.create_table();
    try {
        band.read();
    }
    catch (...) {
        throw;
    }
}

void Vault::get_items(std::vector<BandItem> &items) const {
    get_items_query(SQL_SELECT_ITEMS, items);
}

void Vault::insert_items(std::vector<BandItem> &items) {
    Band band;
    band.insert_items(items);
}

void Vault::get_items_folder(const std::string &folder, std::vector<BandItem> &items) const {
    int sz = snprintf(nullptr, 0, SQL_SELECT_ITEMS_FOLDER, folder.c_str()) + 1;
    char *buf;
    buf = (char*) malloc((size_t) sz);
    snprintf(buf, (size_t) sz, SQL_SELECT_ITEMS_FOLDER, folder.c_str());

    get_items_query(buf, items);
    free(buf);
}

void Vault::get_items_category(const std::string &category, std::vector<BandItem> &items) const {
    int sz = snprintf(nullptr, 0, SQL_SELECT_ITEMS_CATEGORY, category.c_str()) + 1;
    char *buf;
    buf = (char*) malloc((size_t) sz);
    snprintf(buf, (size_t) sz, SQL_SELECT_ITEMS_CATEGORY, category.c_str());

    get_items_query(buf, items);
    free(buf);
}

void Vault::sync() {
    Folder folder;
    try {
        std::vector<FolderItem> folders;
        get_folders(folders);
        folder.sync(folders);
    }
    catch (...) {
        throw;
    }

    Band band;
    try {
        std::vector<BandItem> items;
        get_items(items);
        band.sync(items);
    }
    catch (...) {
        throw;
    }
}

}
