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

#ifndef VAULT_H
#define VAULT_H

#include <vector>

#include "profile.h"
#include "folder.h"
#include "band.h"

namespace OPVault {

class Vault
{
public:
    Vault(const std::string &cloud_data_dir, const std::string &local_data_dir, const std::string &master_password);

private:
    ProfileEntry profile;

    void get_profile();
    void get_items_query(const char query[], std::vector<BandEntry> &items);
    void create_db(const std::string &cloud_data_dir);

public:
    void get_folders(std::vector<FolderEntry> &folders);
    void get_items(std::vector<BandEntry> &items);
    void get_items_folder(std::string folder, std::vector<BandEntry> &items);
    void get_items_category(std::string category, std::vector<BandEntry> &items);
    static void sql_exec(const char sql[]);
};

}

#endif // VAULT_H
