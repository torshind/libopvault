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

#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/sha.h>
#include <cryptopp/sha3.h>

#include "const.h"
#include "profileitem.h"

using namespace CryptoPP;

namespace OPVault {

void ProfileItem::derive_keys(const std::string &master_password) {
    std::string salt;
    StringSource(this->salt, true, new Base64Decoder(new StringSink(salt)));

    PKCS5_PBKDF2_HMAC<SHA512> pbkdf2;
    pbkdf2.DeriveKey(derived_key, KEY_LENGTH, 0,
                     reinterpret_cast<const unsigned char *> (master_password.data()), master_password.length(),
                     reinterpret_cast<const unsigned char *> (salt.data()), salt.length(),
                     iterations);

    try {
        verify_opdata(overviewKey, derived_key);
    }
    catch (...) {
        throw std::invalid_argument("libopvault: wrong password");
    }
}

void ProfileItem::get_profile_key(const std::string &encoded_key_opdata, SecByteBlock &profile_key) {
    std::string opdata_key;
    decrypt_opdata(encoded_key_opdata, derived_key, opdata_key);

    SHA512().CalculateDigest(profile_key, reinterpret_cast<const unsigned char *> (opdata_key.data()), opdata_key.length());
}

void ProfileItem::get_master_key() {
    get_profile_key(masterKey, master_key);
}

void ProfileItem::get_overview_key() {
    get_profile_key(overviewKey, overview_key);
}

}
