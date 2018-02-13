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

#include <fstream>
#include <sstream>
#include <cryptopp/base64.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <sqlite3.h>

#include "dbg.h"
#include "json.hpp"

#include "file.h"

using namespace CryptoPP;

using json = nlohmann::json;

namespace OPVault {

std::string File::directory;

File::~File() {

}

void File::read(const std::string &filename, nlohmann::json &j) {
    std::ifstream ifs(directory + "/" + filename);
    std::string file_string;
    std::ostringstream oss;
    std::string line;

    if (ifs.is_open()) {
        while(getline(ifs, line)) {
            oss << line;
        }
    }
    else {
        throw std::runtime_error(std::string("libopvault: unable to read file ") + directory + "/" + filename);
    }
    file_string = oss.str();

    {
        std::string::iterator it;
        for (it=file_string.begin(); *it!='{'; ++it);
        file_string.erase(file_string.begin(), it);

        for (it=file_string.end(); *it!='}'; --it);
        file_string.erase(++it, file_string.end());
    }

    try {
        j = json::parse(file_string);
    }
    catch (...) {
        throw;
    }

#ifndef NDEBUG
    for (auto it = j.begin(); it != j.end(); ++it) {
        DBGMSG(it.key() << " : " << it.value());
    }
#endif
}

void File::read(const std::string &filename) {
    nlohmann::json j;

    try {
        read(filename, j);
    }
    catch (...) {
        throw;
    }

    for(auto it : j) {
        insert_json(it);
    }
}

void File::read(const std::vector<std::string> &filenames) {
    size_t exept_count = 0;

    for (auto it : filenames) {
        try {
            read(it);
        }
        catch (...) {
            exept_count++;
            if (exept_count == filenames.size()) {
                throw;
            }
            continue;
        }
    }
}

void File::write(const std::string &filename, nlohmann::json &j) {
    std::ofstream ofs(directory + "/" + filename, std::ios::binary);

    if (!ofs.is_open()) {
        throw std::runtime_error(std::string("libopvault: unable to write file ") + directory + "/" + filename);
    }

    try {
        ofs << get_prefix(filename);
    }
    catch (...) {
        throw;
    }

    std::string json_string;
    json_string = j.dump();
    json_string.erase(0, 1).erase(json_string.end()-1, json_string.end());
    ofs << json_string << "});";

    ofs.close();
}

void File::append(const std::string &filename, nlohmann::json &j) {
    std::fstream iofs(directory + "/" + filename, std::ios::in | std::ios::out | std::ios::binary);

    if (iofs.is_open()) {
        iofs.seekg(-1, std::ios::end);
        while (iofs.get() != '}' && iofs.tellg() > 0) {
            iofs.seekg(-2, std::ios::cur);
        }
        if (iofs.tellg() > 0) {
            iofs.seekg(-1, std::ios::cur);
            iofs.seekp(iofs.tellg());
            iofs.seekg(-1, std::ios::cur);
            if (iofs.get() != '{') {
                iofs << ",";
            }
        } else {
            iofs.clear();
            try {
                iofs << get_prefix(filename);
            }
            catch (...) {
                throw;
            }
        }
    }
    else {
        iofs.open(directory + "/" + filename, std::ios::out);
        if (!iofs.is_open()) {
            throw std::runtime_error(std::string("libopvault: unable to write file ") + directory + "/" + filename);
        }
        try {
            iofs << get_prefix(filename);
        }
        catch (...) {
            throw;
        }
    }
    std::string json_string;
    json_string = j.dump();
    json_string.erase(0, 1).erase(json_string.end()-1, json_string.end());
    iofs << json_string << "});";

    iofs.close();
}

void File::sql_exec(const char sql[]) {
    sqlite3 *db;
    char *zErrMsg = nullptr;
    int rc;

    rc = sqlite3_open(DBFILE, &db);

    if(rc){
        std::ostringstream os;
        os << "libopvault: can't open database: " << sqlite3_errmsg(db) << " - error code: " << rc;
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    }

    rc = sqlite3_exec(db, sql, nullptr, nullptr, &zErrMsg);

    if(rc != SQLITE_OK){
        std::ostringstream os;
        os << "libopvault: SQL error: " << zErrMsg << " - error code: " << rc;
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    }

    sqlite3_free(zErrMsg);
    sqlite3_close(db);

}

void File::insert_json(nlohmann::json &j) {
    BaseItem* item;
    try {
        item = json2item(j);
    }
    catch (...) {
        throw;
    }
    insert_item(item);
    delete item;
}

std::string File::get_prefix(const std::string &filename) {
    if (filename.find("band_") != std::string::npos) {
        return "ld({";
    } else if (filename.compare("folders.js") == 0) {
        return "loadFolders({";
    } else {
        throw std::runtime_error("libopvault: Unknown file type error");
    }
}

void File::sql_update_long(const std::string &table, const std::string &col, const std::string &uuid, long val) {
    int sz = snprintf(nullptr, 0, SQL_UPDATE_LONG,
                      table.c_str(),
                      col.c_str(),
                      val,
                      uuid.c_str()) + 1;
    char *buf;
    buf = (char*) malloc((size_t) sz);
    snprintf(buf, (size_t) sz, SQL_UPDATE_LONG,
             table.c_str(),
             col.c_str(),
             val,
             uuid.c_str());

    DBGVAR(uuid);
    DBGVAR(val);

    sql_exec(buf);
    free(buf);
}

}
