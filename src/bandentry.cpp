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

#include <uuid/uuid.h>

#include "dbg.h"
#include "const.h"
#include "vault.h"

#include "bandentry.h"

using namespace std;
using namespace CryptoPP;

namespace OPVault {

void BandEntry::verify_key() {
    // TODO: verify key using master mac key
}

void BandEntry::decrypt_key(std::string &key) {
    string encrypted_key;
    StringSource(k, true, new Base64Decoder(new StringSink(encrypted_key)));

    const byte* key_byte = (byte*) encrypted_key.data();

    byte iv[IV_LENGTH] = "";
    memcpy(iv, key_byte, IV_LENGTH*sizeof(byte));

    byte *ciphertext = NULL;
    ciphertext = (byte*) calloc(ITEM_KEY_LENGTH, sizeof(byte));
    memcpy(ciphertext, key_byte+IV_LENGTH, ITEM_KEY_LENGTH*sizeof(byte));

    AES::Decryption aes_decryption(master_key, ENC_KEY_LENGTH);
    CBC_Mode_ExternalCipher::Decryption cbc_decryption(aes_decryption, iv);

    StreamTransformationFilter stf_decryptor(cbc_decryption, new StringSink(key), StreamTransformationFilter::NO_PADDING);

    stf_decryptor.Put((const byte *)ciphertext, ITEM_KEY_LENGTH);
    stf_decryptor.MessageEnd();
}

void BandEntry::decrypt_data(std::string& data) {
    string decrypted_data;
    string item_key;

    decrypt_key(item_key);

    decrypt_opdata(d, reinterpret_cast<const unsigned char *> (item_key.data()), data);
}

void BandEntry::decrypt_overview(std::string& overview) {
    string decrypted_overview;

    decrypt_opdata(o, overview_key, overview);
}

void BandEntry::verify() {
    // TODO: verify item using hmac and overview mac key
}

void BandEntry::init() {
    // Generate UUID
    uuid_t uuid_bin;
    char uuid_str[37];

    uuid_generate(uuid_bin);
    uuid_unparse_upper(uuid_bin, uuid_str);
    uuid = string(uuid_str);
    uuid.erase(std::remove(uuid.begin(), uuid.end(), '-'), uuid.end());

    // Generate key
    AutoSeededRandomPool prng;
    string encrypted_key;

    SecByteBlock plain_key(ITEM_KEY_LENGTH);
    prng.GenerateBlock(plain_key, plain_key.size());

    SecByteBlock iv(IV_LENGTH);
    prng.GenerateBlock(iv, iv.size());

    // Encryption
    AES::Encryption aes_encryption(master_key, ENC_KEY_LENGTH);
    CBC_Mode_ExternalCipher::Encryption cbc_encryption(aes_encryption, iv);

    StreamTransformationFilter stf_encryptor(cbc_encryption, new StringSink(encrypted_key), StreamTransformationFilter::NO_PADDING);
    stf_encryptor.Put(plain_key, ITEM_KEY_LENGTH);
    stf_encryptor.MessageEnd();

    // HMAC
    string mac;

    HMAC<SHA256> hmac(plain_key+ENC_KEY_LENGTH, MAC_KEY_LENGTH);

    StringSource(string(reinterpret_cast<const char *> (iv.data()), IV_LENGTH) + encrypted_key, true, new HashFilter(hmac, new StringSink(mac)));

    // Base64 encoding
    StringSource(string(reinterpret_cast<const char *> (iv.data()), IV_LENGTH) + encrypted_key + mac, true, new Base64Encoder(new StringSink(k)));

    // TODO: timestamps
}

void BandEntry::set_data(string _d) {
    updateState = true;
    if (k.empty()) {
        init();
    }

    string item_key;
    unsigned char iv[IV_LENGTH];

    decrypt_key(item_key);

    if (d.empty()) {
        // Generate iv
        AutoSeededRandomPool prng;
        prng.GenerateBlock(iv, IV_LENGTH);
    } else {
        get_iv(d, iv);
    }

    encrypt_opdata(_d, iv, reinterpret_cast<const unsigned char *> (item_key.data()), d);
}

}
