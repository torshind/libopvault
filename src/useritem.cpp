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

#include <cryptopp/osrng.h>

#include <uuid/uuid.h>

#include "useritem.h"

using namespace CryptoPP;

namespace OPVault {

void UserItem::decrypt_overview(std::string& overview) {
    if (!o.empty()) {
        decrypt_opdata(o, overview_key, overview);
    }
}

void UserItem::init() {
    created = time(nullptr);

    // Generate UUID
    uuid_t uuid_bin;
    char uuid_str[37];

    uuid_generate(uuid_bin);
    uuid_unparse_upper(uuid_bin, uuid_str);
    uuid = std::string(uuid_str);
    uuid.erase(std::remove(uuid.begin(), uuid.end(), '-'), uuid.end());
}

void UserItem::setup_update() {
    updated = time(nullptr);
    updateState = true;
    if (uuid.empty()) {
        init();
    }
}

void UserItem::set_overview(const std::string &_o) {
    setup_update();

    SecByteBlock iv;

    // Generate iv
    AutoSeededRandomPool prng;
    iv = SecByteBlock(AES::BLOCKSIZE);
    prng.GenerateBlock(iv, AES::BLOCKSIZE);

    if (!o.empty()) {
        o.clear();
    }

    encrypt_opdata(_o, iv, overview_key, o);
}

}
