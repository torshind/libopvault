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

using namespace std;
using namespace CryptoPP;

using json = nlohmann::json;

namespace OPVault {

std::string File::directory;

void File::read(const std::string &filename) {
    ifstream ifs(directory + "/" + filename);
    string file_string;
    ostringstream oss;
    string line;

    if (ifs.is_open()) {
        while(getline(ifs, line)) {
            oss << line;
        }
    }
    else {
        throw std::runtime_error(string("libopvault: unable to read file ") + directory + "/" + filename);
    }
    file_string = oss.str();

    {
        string::iterator it;
        for (it=file_string.begin(); *it!='{'; ++it);
        file_string.erase(file_string.begin(), it);

        for (it=file_string.end(); *it!='}'; --it);
        file_string.erase(++it, file_string.end());
    }

    try {
        data = json::parse(file_string);
    }
    catch (...) {
        throw;
    }

#ifndef NDEBUG
    for (auto it = data.begin(); it != data.end(); ++it) {
        DBGMSG(it.key() << " : " << it.value());
    }
#endif

    return;
}

void File::sql_exec(const char sql[]) {
    sqlite3 *db;
    char *zErrMsg = nullptr;
    int rc;

    rc = sqlite3_open(DBFILE, &db);

    if(rc){
        ostringstream os;
        os << "libopvault: can't open database: " << sqlite3_errmsg(db) << " - error code: " << rc;
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    }

    rc = sqlite3_exec(db, sql, nullptr, nullptr, &zErrMsg);

    if(rc != SQLITE_OK){
        ostringstream os;
        os << "libopvault: SQL error: " << zErrMsg << " - error code: " << rc;
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        throw std::runtime_error(os.str());
    }

    sqlite3_free(zErrMsg);
    sqlite3_close(db);

}

}
