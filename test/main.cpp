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

#include <iostream>
#include <experimental/filesystem>
#include <sqlite3.h>

#include "vault.h"
#include "profile.h"
#include "folder.h"
#include "band.h"
#include "baseitem.h"


const char SQL_UPDATE_LONG_ALL[] = "UPDATE %s SET %s = %ld;";

using namespace std;
using namespace OPVault;

void sql_exec(const char sql[]) {
    sqlite3 *db;
    char *zErrMsg = nullptr;
    int rc;

    rc = sqlite3_open(DBFILE, &db);

    if(rc){
        std::ostringstream os;
        os << "Can't open database: " << sqlite3_errmsg(db) << " - error code: " << rc;
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    }

    rc = sqlite3_exec(db, sql, nullptr, nullptr, &zErrMsg);

    if(rc != SQLITE_OK){
        std::ostringstream os;
        os << "SQL error: " << zErrMsg << " - error code: " << rc;
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    }

    sqlite3_free(zErrMsg);
    sqlite3_close(db);

}

void sql_update_long(const std::string &table, const std::string &col, long val) {
    int sz = snprintf(nullptr, 0, SQL_UPDATE_LONG_ALL,
                      table.c_str(),
                      col.c_str(),
                      val) + 1;
    char *buf;
    buf = (char*) malloc((size_t) sz);
    snprintf(buf, (size_t) sz, SQL_UPDATE_LONG_ALL,
             table.c_str(),
             col.c_str(),
             val);

    sql_exec(buf);
    free(buf);
}

static void get_items(const Vault &vault) {
    vector<BandItem> items;

    vault.get_items(items);
    for(auto &item : items) {
        cout << "Item " << item.get_uuid() << endl;
        string str;
        item.decrypt_overview(str);
        cout << "Overview: " << str << endl;
        item.decrypt_data(str);
        cout << "Data: " << str << endl;
        cout << "Category: " << item.get_category() << endl;
        cout << "Fave: " << item.get_fave() << endl;
        cout << "Folder: " << item.get_folder() << endl;
        cout << "Trashed: " << item.get_trashed() << endl;
    }

}

static void get_folders(const Vault &vault) {
    vector<FolderItem> folders;

    vault.get_folders(folders);
    for(auto &folder : folders) {
        cout << "Folder " << folder.get_uuid() << endl;
        string str;
        folder.decrypt_overview(str);
        cout << "Overview: " << str << endl;
    }
}

static void get_items_folder(const Vault &vault) {
    vector<FolderItem> folders;
    vector<BandItem> items;

    vault.get_folders(folders);

    for(auto &folder : folders) {
        cout << "Folder " << folder.get_uuid() << endl;
        items.clear();
        vault.get_items_folder(folder.get_uuid(), items);
        for(auto &item : items) {
            cout << "Item " << item.get_uuid() << endl;
            string str;
            item.decrypt_overview(str);
            cout << "Overview: " << str << endl;
            item.decrypt_data(str);
            cout << "Data: " << str << endl;
        }
    }

}

static void get_items_category(const Vault &vault) {
    vector<BandItem> items;

    for(auto &cat : CATEGORIES) {
        cout << "Category " << cat.second << endl;
        items.clear();
        vault.get_items_category(cat.first, items);
        for(auto &item : items) {
            cout << "Item " << item.get_uuid() << endl;
            string str;
            item.decrypt_overview(str);
            cout << "Overview: " << str << endl;
            item.decrypt_data(str);
            cout << "Data: " << str << endl;
        }
    }

}

int main(int argc, char *argv[])
{
    string master_password = u8"freddy";
    string cloud_data_dir = "./onepassword_data/default";
    string cloud_sync_test_data_dir = "./onepassword_data/sync_test";
    string local_data_dir = "./";

    {
        // OPEN VAULT
        Vault vault(cloud_data_dir, local_data_dir, master_password);

        // GET FOLDER CONTENTS
        get_folders(vault);


        // GET ITEMS BY FOLDER
        get_items_folder(vault);

        // GET ITEMS BY CATEGORY
        get_items_category(vault);

        // GET ALL ITEMS
        get_items(vault);
    }

    {
        // OPEN VAULT
        Vault vault(cloud_data_dir, local_data_dir, master_password);

        vector<FolderItem> folders;
        vector<BandItem> items;

        // INSERT NEW ITEMS
        items.clear();
        BandItem item1;
        item1.set_category("001");
        items.push_back(item1);
        BandItem item2;
        item2.set_data("{DATA2}");
        items.push_back(item2);
        BandItem item3;
        item3.set_folder("FOLDER3");
        items.push_back(item3);
        BandItem item4;
        item4.set_overview("{OVERVIEW4}");
        items.push_back(item4);
        BandItem item5;
        item5.set_fave(5000);
        items.push_back(item5);
        BandItem item6;
        item6.set_trashed(1);
        items.push_back(item6);
        BandItem item7;
        item7.set_category("099");
        item7.set_data("{DATA7}");
        item7.set_overview("{OVERVIEW7}");
        items.push_back(item7);
        vault.insert_items(items);

        // INSERT NEW FOLDER
        FolderItem folder;
        folder.set_overview("{\"title\":\"Mordor\"}");
        folders.push_back(folder);
        vault.insert_folders(folders);

        // CHECK NEW DATA
        get_folders(vault);
        get_items(vault);

        // MODIFY ITEMS
        items[0].set_data("{DATA1.1}");
        items[1].set_category("002");
        items[1].set_data("{DATA2.2}");
        items[2].set_overview("{OVERVIEW3.3}");
        items[3].set_data("{DATA4.4}");
        items[3].set_fave(4000);
        items[3].set_overview("{OVERVIEW4.4}");
        items[4].set_trashed(1);
        items[5].set_category("005");
        items[5].set_data("{DATA6.6}");
        items[5].set_fave(6000);
        items[5].set_folder("FOLDER6");
        items[5].set_overview("{OVERVIEW6.6}");
        items[5].set_trashed(0);
        items[6].set_category("111");
        items[6].set_data("{DATA7.7}");
        items[6].set_overview("{OVERVIEW7.7}");

        vault.insert_items(items);

        // CHECK MODIFIED DATA
        get_folders(vault);
        get_items(vault);
    }

    std::experimental::filesystem::copy(cloud_data_dir, cloud_sync_test_data_dir, std::experimental::filesystem::copy_options::recursive);

    {
        // SYNC TEST
        Vault vault(cloud_sync_test_data_dir, local_data_dir, master_password);

        // CHECK SYNCED DATA
        get_folders(vault);
        get_items(vault);
    }

    // RESET LOCAL DB
    remove("./opvault.db");

    {
        // OPEN SYNCED VAULT
        Vault vault(cloud_sync_test_data_dir, local_data_dir, master_password);

        // CHECK SYNCED DATA
        get_folders(vault);
        get_items(vault);
    }

    // RESET UPDATED TO FUTURE TO FORCE UPDATE TO CLOUD
    sql_update_long("Items", "updated", LONG_MAX);
    sql_update_long("Folders", "updated", LONG_MAX);

    {
        // OPEN SYNCED VAULT
        Vault vault(cloud_sync_test_data_dir, local_data_dir, master_password);

        // CHECK SYNCED DATA
        get_folders(vault);
        get_items(vault);
    }

    // RESET LOCAL DB
    remove("./opvault.db");

    {
        // OPEN SYNCED VAULT
        Vault vault(cloud_sync_test_data_dir, local_data_dir, master_password);

        // CHECK SYNCED DATA
        get_folders(vault);
        get_items(vault);
    }

    // RESET TX TO 0 TO FORCE MERGE FROM CLOUD
    sql_update_long("Items", "tx", 0);
    sql_update_long("Folders", "tx", 0);

    {
        // OPEN SYNCED VAULT
        Vault vault(cloud_sync_test_data_dir, local_data_dir, master_password);

        // CHECK SYNCED DATA
        get_folders(vault);
        get_items(vault);
    }

    // RESET LOCAL DB
    remove("./opvault.db");

    {
        // OPEN SYNCED VAULT
        Vault vault(cloud_sync_test_data_dir, local_data_dir, master_password);

        // CHECK SYNCED DATA
        get_folders(vault);
        get_items(vault);
    }

    // RESET LOCAL DB
    remove("./opvault.db");
    // RESET SYNCED DATA
    std::experimental::filesystem::remove_all(cloud_sync_test_data_dir);

    return 0;
}
