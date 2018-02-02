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

#include "vault.h"
#include "profile.h"
#include "folder.h"
#include "band.h"
#include "baseitem.h"

using namespace std;
using namespace OPVault;

static void get_items(const Vault &vault) {
    vector<BandItem> items;

    vault.get_items(items);
    for(auto it=items.begin(); it!=items.end(); ++it) {
        cout << "Item " << it->get_uuid() << endl;
        string str;
        it->decrypt_overview(str);
        cout << "Overview: " << str << endl;
        it->decrypt_data(str);
        cout << "Data: " << str << endl;
        cout << "Category: " << it->get_category() << endl;
        cout << "Fave: " << it->get_fave() << endl;
        cout << "Folder: " << it->get_folder() << endl;
        cout << "Trashed: " << it->get_trashed() << endl;
    }

}

static void get_folders(const Vault &vault) {
    vector<FolderItem> folders;

    vault.get_folders(folders);
    for(auto it=folders.begin(); it!=folders.end(); ++it) {
        cout << "Folder " << it->get_uuid() << endl;
        string str;
        it->decrypt_overview(str);
        cout << "Overview: " << str << endl;
    }
}

static void get_items_folder(const Vault &vault) {
    vector<FolderItem> folders;
    vector<BandItem> items;

    vault.get_folders(folders);

    for(auto it1=folders.begin(); it1!=folders.end(); ++it1) {
        cout << "Folder " << it1->get_uuid() << endl;
        items.clear();
        vault.get_items_folder(it1->get_uuid(), items);
        for(auto it2=items.begin(); it2!=items.end(); ++it2) {
            cout << "Item " << it2->get_uuid() << endl;
            string str;
            it2->decrypt_overview(str);
            cout << "Overview: " << str << endl;
            it2->decrypt_data(str);
            cout << "Data: " << str << endl;
        }
    }

}

static void get_items_category(const Vault &vault) {
    vector<BandItem> items;

    for(auto it1=CATEGORIES.begin(); it1!=CATEGORIES.end(); ++it1) {
        cout << "Category " << it1->second << endl;
        items.clear();
        vault.get_items_category(it1->first, items);
        for(auto it2=items.begin(); it2!=items.end(); ++it2) {
            cout << "Item " << it2->get_uuid() << endl;
            string str;
            it2->decrypt_overview(str);
            cout << "Overview: " << str << endl;
            it2->decrypt_data(str);
            cout << "Data: " << str << endl;
        }
    }

}

int main(int argc, char *argv[])
{
    string master_password = u8"freddy";
    string cloud_data_dir = "./onepassword_data/default";
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
        vault.set_folders(folders);

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

    {
        // OPEN VAULT
        Vault vault(cloud_data_dir, local_data_dir, master_password);

        vault.sync();
    }

    // RESET LOCAL DB
    remove("./opvault.db");

    return 0;
}
