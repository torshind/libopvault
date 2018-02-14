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

#ifndef FILE_H
#define FILE_H

#include <string>

#include "json.hpp"
#include "const.h"
#include "useritem.h"

namespace OPVault {

class File
{
protected:
    File() {}
    virtual ~File();

    static std::string directory;

    void read(const std::string &filename, nlohmann::json &j);
    void read(const std::string &filename);
    void read(const std::vector<std::string> &filenames);
    void write(const std::string &filename, nlohmann::json &j);
    void append(const std::string &filename, nlohmann::json &j);
    void append(const std::string &filename, UserItem* user_item);
    void sync(const std::string filename, std::unordered_map<std::string, UserItem*> &local_map);
    void sync(const std::vector<std::string> &filenames, std::unordered_map<std::string, UserItem*> &local_map);

    void sql_exec(const char sql[]);
    void sql_update_long(const std::string &table, const std::string &col, const std::string &uuid, long val);

    void insert_json(nlohmann::json &j);
    virtual BaseItem* json2item(nlohmann::json &j) = 0;
    virtual void insert_item(BaseItem* base_item) = 0;
    virtual void update_tx(BaseItem* base_item) = 0;

public:
    void set_directory(const std::string &d) { directory = d; }

private:
    std::string get_prefix(const std::string &filename);
};

}

#endif // FILE_H
