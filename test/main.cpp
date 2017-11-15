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

using namespace std;
using namespace OPVault;

int main(int argc, char *argv[])
{
    string master_password = u8"freddy";
    string cloud_data_dir = "./onepassword_data/default";
    string local_data_dir = "./";

    Vault vault(cloud_data_dir, local_data_dir, master_password);

    vector<FolderEntry> folders;
    vault.get_folders(folders);
    for(vector<FolderEntry>::iterator it=folders.begin(); it!=folders.end(); ++it) {
        it->decrypt_overview();
    }

    vector<BandEntry> items;
    vault.get_items(items);
    for(vector<BandEntry>::iterator it=items.begin(); it!=items.end(); ++it) {
        it->decrypt_overview();
        it->decrypt_data();
    }

    for(vector<FolderEntry>::iterator it1=folders.begin(); it1!=folders.end(); ++it1) {
        cout << "Folder " << it1->uuid << endl;
        items.clear();
        vault.get_items_folder(it1->uuid, items);
        for(vector<BandEntry>::iterator it2=items.begin(); it2!=items.end(); ++it2) {
            cout << "Item " << it2->get_uuid() << endl;
            it2->decrypt_overview();
            it2->decrypt_data();
        }
    }

    for(unordered_map<string,string>::const_iterator it1=CATEGORIES.begin(); it1!=CATEGORIES.end(); ++it1) {
        cout << "Category " << it1->second << endl;
        items.clear();
        vault.get_items_category(it1->first, items);
        for(vector<BandEntry>::iterator it2=items.begin(); it2!=items.end(); ++it2) {
            cout << "Item " << it2->get_uuid() << endl;
            it2->decrypt_overview();
            it2->decrypt_data();
        }
    }

    return 0;
}
