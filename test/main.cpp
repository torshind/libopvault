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
#include "baseentry.h"

using namespace std;
using namespace OPVault;

int main(int argc, char *argv[])
{
    string master_password = u8"freddy";
    string cloud_data_dir = "./onepassword_data/default";
    string local_data_dir = "./";

    // OPEN VAULT
    Vault vault(cloud_data_dir, local_data_dir, master_password);

    // GET FOLDER CONTENTS
    vector<FolderEntry> folders;
    vault.get_folders(folders);
    for(vector<FolderEntry>::iterator it=folders.begin(); it!=folders.end(); ++it) {
        cout << "Folder " << it->get_uuid() << endl;
        string str;
        it->decrypt_overview(str);
        cout << "Overview: " << str << endl;
    }

    vector<BandEntry> items;

    // GET ITEMS BY FOLDER
    for(vector<FolderEntry>::iterator it1=folders.begin(); it1!=folders.end(); ++it1) {
        cout << "Folder " << it1->get_uuid() << endl;
        items.clear();
        vault.get_items_folder(it1->get_uuid(), items);
        for(vector<BandEntry>::iterator it2=items.begin(); it2!=items.end(); ++it2) {
            cout << "Item " << it2->get_uuid() << endl;
            string str;
            it2->decrypt_overview(str);
            cout << "Overview: " << str << endl;
            it2->decrypt_data(str);
            cout << "Data: " << str << endl;
        }
    }

    // GET ITEMS BY CATEGORY
    for(unordered_map<string,string>::const_iterator it1=CATEGORIES.begin(); it1!=CATEGORIES.end(); ++it1) {
        cout << "Category " << it1->second << endl;
        items.clear();
        vault.get_items_category(it1->first, items);
        for(vector<BandEntry>::iterator it2=items.begin(); it2!=items.end(); ++it2) {
            cout << "Item " << it2->get_uuid() << endl;
            string str;
            it2->decrypt_overview(str);
            cout << "Overview: " << str << endl;
            it2->decrypt_data(str);
            cout << "Data: " << str << endl;
        }
    }

    // GET ALL ITEMS
    items.clear();
    vault.get_items(items);
    for(vector<BandEntry>::iterator it=items.begin(); it!=items.end(); ++it) {
        cout << "Item " << it->get_uuid() << endl;
        string str;
        it->decrypt_overview(str);
        cout << "Overview: " << str << endl;
        it->decrypt_data(str);
        cout << "Data: " << str << endl;
    }

    // INSERT NEW ITEM
    items.clear();
    BandEntry item1;
    item1.set_category("001");
    items.push_back(item1);
    BandEntry item2;
    item2.set_data("{DATA2}");
    items.push_back(item2);
    BandEntry item3;
    item3.set_folder("FOLDER3");
    items.push_back(item3);
    BandEntry item4;
    item4.set_overview("{OVERVIEW4}");
    items.push_back(item4);
    BandEntry item5;
    item5.set_fave(5000);
    items.push_back(item5);
    BandEntry item6;
    item6.set_trashed(1);
    items.push_back(item6);
    BandEntry item7;
    item7.set_category("099");
    item7.set_data("{DATA7}");
    item7.set_overview("{OVERVIEW7}");
    items.push_back(item7);
    vault.set_items(items);

    // INSERT NEW FOLDER
    FolderEntry folder;
    folder.set_overview("{\"title\":\"Mordor\"}");
    folders.push_back(folder);
    vault.set_folders(folders);

    // CHECK NEW DATA
    folders.clear();
    vault.get_folders(folders);
    for(vector<FolderEntry>::iterator it=folders.begin(); it!=folders.end(); ++it) {
        cout << "Folder " << it->get_uuid() << endl;
        string str;
        it->decrypt_overview(str);
        cout << "Overview: " << str << endl;
    }
    items.clear();
    vault.get_items(items);
    for(vector<BandEntry>::iterator it=items.begin(); it!=items.end(); ++it) {
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

    // RESET LOCAL DB
    remove("./opvault.db");

    return 0;
}
