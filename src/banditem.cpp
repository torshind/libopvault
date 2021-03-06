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

#include <cryptopp/base64.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>

#include "dbg.h"
#include "const.h"
#include "vault.h"

#include "banditem.h"

using namespace CryptoPP;

namespace OPVault {

void BandItem::decrypt_key(SecByteBlock &key) {
    std::string encrypted_key;
    StringSource(k, true, new Base64Decoder(new StringSink(encrypted_key)));

    // Verify
    HMAC<SHA256> hmac(master_key.data()+ENC_KEY_LENGTH, MAC_KEY_LENGTH);
    const int flags = HashVerificationFilter::THROW_EXCEPTION | HashVerificationFilter::HASH_AT_END;

    try {
        StringSource(encrypted_key, true, new HashVerificationFilter(hmac, nullptr, flags));
    }
    catch (...) {
        throw std::invalid_argument("libopvault: wrong password");
    }

    // Decrypt
    SecByteBlock iv(reinterpret_cast<const unsigned char *> (encrypted_key.data()), AES::BLOCKSIZE);

    std::string ciphertext = std::string(encrypted_key.data()+AES::BLOCKSIZE, ITEM_KEY_LENGTH);

    CBC_Mode<AES>::Decryption decryption(master_key, ENC_KEY_LENGTH, iv);

    key = SecByteBlock(ITEM_KEY_LENGTH);
    StringSource(ciphertext, true, new StreamTransformationFilter(decryption, new ArraySink(key.data(), key.size()), StreamTransformationFilter::NO_PADDING));
}

void BandItem::decrypt_data(std::string& data) {
    if (!d.empty()) {
        verify();
        SecByteBlock item_key;

        decrypt_key(item_key);

        decrypt_opdata(d, item_key, data);
    }
}

void BandItem::to_json(nlohmann::json &j) {
    nlohmann::json j_item;
    j_item["created"]  = created;
    j_item["o"]        = o;
    j_item["tx"]       = tx;
    j_item["updated"]  = updated;
    j_item["uuid"]     = uuid;
    j_item["category"] = category;
    j_item["d"]        = d;
    j_item["fave"]     = fave;
    j_item["folder"]   = folder;
    j_item["hmac"]     = hmac;
    j_item["k"]        = k;
    if (trashed == 0) {
        j_item["trashed"] = false;
    } else if (trashed == 1) {
        j_item["trashed"] = true;
    }

    j[uuid] = j_item;
}

std::string BandItem::get_hmac_input_str() {
    return std::string(
                "category" + category +
                "created" + std::to_string(created) +
                "d" + d +
                (fave == -1 ? "" : "fave" + std::to_string(fave)) +
                (folder == "" ? "" : "folder" + folder) +
                "k" + k +
                "o" + o +
                (trashed == -1 ? "" : "trashed" + std::to_string(trashed)) +
                "tx" + std::to_string(tx) +
                "updated" + std::to_string(updated) +
                "uuid" + uuid
                );
}

void BandItem::verify() {
    std::string input;
    StringSource(hmac, true, new Base64Decoder(new StringSink(input)));

    HMAC<SHA256> _hmac(overview_key.data()+ENC_KEY_LENGTH, MAC_KEY_LENGTH);

    input = get_hmac_input_str() + input;

    const int flags = HashVerificationFilter::THROW_EXCEPTION | HashVerificationFilter::HASH_AT_END;

    StringSource(input, true, new HashVerificationFilter(_hmac, nullptr, flags));
}

void BandItem::init() {
    UserItem::init();

    // Generate key
    AutoSeededRandomPool prng;
    std::string encrypted_key;

    SecByteBlock plain_key(ITEM_KEY_LENGTH);
    prng.GenerateBlock(plain_key, plain_key.size());

    SecByteBlock iv(AES::BLOCKSIZE);
    prng.GenerateBlock(iv, iv.size());

    // Encryption
    CBC_Mode<AES>::Encryption encryption(master_key, ENC_KEY_LENGTH, iv);

    ArraySource(plain_key.data(), plain_key.size(), true, new StreamTransformationFilter(encryption, new StringSink(encrypted_key), StreamTransformationFilter::NO_PADDING));

    // HMAC
    std::string mac;

    HMAC<SHA256> _hmac(master_key+ENC_KEY_LENGTH, MAC_KEY_LENGTH);

    StringSource(std::string(reinterpret_cast<const char *> (iv.data()), AES::BLOCKSIZE) + encrypted_key, true, new HashFilter(_hmac, new StringSink(mac)));

    // Base64 encoding
    StringSource(std::string(reinterpret_cast<const char *> (iv.data()), AES::BLOCKSIZE) + encrypted_key + mac, true, new Base64Encoder(new StringSink(k), false));
}

void BandItem::set_category(const std::string &_category) {
    setup_update();

    category = _category;
}

void BandItem::set_data(const std::string &_d) {
    setup_update();

    SecByteBlock item_key;
    SecByteBlock iv;

    decrypt_key(item_key);

    // Generate iv
    AutoSeededRandomPool prng;
    iv = SecByteBlock(AES::BLOCKSIZE);
    prng.GenerateBlock(iv, AES::BLOCKSIZE);

    if (!d.empty()) {
        d.clear();
    }

    encrypt_opdata(_d, iv, item_key, d);
}

void BandItem::set_fave(const long _fave) {
    setup_update();

    fave = _fave;
}

void BandItem::set_folder(const std::string &_folder) {
    setup_update();

    folder = _folder;
}

void BandItem::set_trashed(const int _trashed) {
    setup_update();

    trashed = _trashed;
}

void BandItem::generate_hmac() {
    // HMAC
    std::string mac;
    std::string input = get_hmac_input_str();

    HMAC<SHA256> _hmac(overview_key.data()+ENC_KEY_LENGTH, MAC_KEY_LENGTH);

    StringSource(input, true, new HashFilter(_hmac, new StringSink(mac)));

    // Base64 encoding
    if (!hmac.empty()) {
        hmac.clear();
    }
    StringSource(mac, true, new Base64Encoder(new StringSink(hmac), false));
}

}
