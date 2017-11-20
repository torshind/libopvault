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
        string str;
        it->decrypt_overview(str);
        cout << "Overview: " << str << endl;
        it->decrypt_data(str);
        cout << "Data: " << str << endl;
    }

    // INSERT NEW ITEM
    BandEntry item;
    item.set_category(CATEGORIES.find("001")->first);
    item.set_data("{\"sections\":[{\"name\":\"\",\"title\":\"\",\"fields\":[{\"k\":\"string\",\"n\":\"org_name\",\"t\":\"group\"},{\"k\":\"URL\",\"n\":\"website\",\"v\":\"http://www.urbana.edu/resources/community/johnny-appleseed/appleseed-society.html\",\"t\":\"website\"},{\"k\":\"phone\",\"n\":\"phone\",\"t\":\"telephone\"},{\"k\":\"string\",\"n\":\"member_name\",\"v\":\"Wendy Appleseed\",\"t\":\"member name\"},{\"k\":\"monthYear\",\"n\":\"member_since\",\"t\":\"member since\"},{\"k\":\"monthYear\",\"n\":\"expiry_date\",\"v\":2625,\"t\":\"expiry date\"},{\"k\":\"string\",\"n\":\"membership_no\",\"v\":\"123456789\",\"t\":\"member ID\"},{\"k\":\"concealed\",\"n\":\"pin\",\"v\":\"B8HqCdCMAY8KxJqgWASD\",\"t\":\"password\"}]}]}");
    item.set_folder("379A3A7E5D5A47A6AA3A69C4D1E57D1B");
    item.set_overview("{\"title\":\"Tumblr\",\"URLs\":[{\"u\":\"http://www.tumblr.com/login\"}],\"ainfo\":\"peter@appleseed.com\",\"url\":\"http://www.tumblr.com/login\",\"tags\":[\"Sample\"],\"ps\":48}]");
    items.push_back(item);
    vault.set_items(items);

    return 0;
}
