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

#include "baseentry.h"

using namespace std;
using namespace CryptoPP;

namespace OPVault {

unsigned char BaseEntry::derived_key[KEY_LENGTH];
unsigned char BaseEntry::master_key[KEY_LENGTH];
unsigned char BaseEntry::overview_key[KEY_LENGTH];

void BaseEntry::verify_opdata(const string &encoded_opdata, const unsigned char key[]) {
    string opdata;
    StringSource(encoded_opdata, true, new Base64Decoder(new StringSink(opdata)));

    HMAC<SHA256> hmac(key+ENC_KEY_LENGTH, MAC_KEY_LENGTH);
    const int flags = HashVerificationFilter::THROW_EXCEPTION | HashVerificationFilter::HASH_AT_END;

    StringSource(opdata, true, new HashVerificationFilter(hmac, NULL, flags));
}

void BaseEntry::decrypt_opdata(const std::string &encoded_opdata, const unsigned char key[], std::string &plaintext) {
    try {
        verify_opdata(encoded_opdata, key);
    }
    catch (...) {
        throw std::invalid_argument("libopvault: failed hash check");
    }

    string opdata;
    StringSource(encoded_opdata, true, new Base64Decoder(new StringSink(opdata)));

    DBGVAR(opdata);

    const byte* opdata_byte = (byte*) opdata.data();

    int ciphertext_length = 0;
    ciphertext_length = opdata.length() - NON_CIPHER_LENGTH;

    byte header[HEADER_LENGTH] = "";
    memcpy(header, opdata_byte, HEADER_LENGTH*sizeof(byte));

    if (memcmp(header, "opdata01", HEADER_LENGTH*sizeof(byte))) {
        throw std::invalid_argument("libopvault: invalid opdata value");
        return;
    }

    int64_t plaintext_length = 0;
    memcpy(&plaintext_length, opdata_byte+HEADER_LENGTH, LENGTH_LENGTH*sizeof(byte));

    byte iv[IV_LENGTH] = "";
    memcpy(iv, opdata_byte+START_IV, IV_LENGTH*sizeof(byte));

    byte *ciphertext = NULL;
    ciphertext = (byte*) calloc(ciphertext_length, sizeof(byte));
    memcpy(ciphertext, opdata_byte+START_CIPHER, ciphertext_length*sizeof(byte));

    AES::Decryption aes_decryption(key, ENC_KEY_LENGTH);
    CBC_Mode_ExternalCipher::Decryption cbc_decryption(aes_decryption, iv);

    StreamTransformationFilter stf_decryptor(cbc_decryption, new StringSink(plaintext), StreamTransformationFilter::NO_PADDING);

    stf_decryptor.Put((const byte *)ciphertext, ciphertext_length);
    stf_decryptor.MessageEnd();

    plaintext.erase(0, ciphertext_length-plaintext_length);

    DBGVAR(plaintext);
}

void BaseEntry::get_iv(const std::string &encoded_opdata, unsigned char iv[IV_LENGTH]) {
    string opdata;
    StringSource(encoded_opdata, true, new Base64Decoder(new StringSink(opdata)));

    memcpy(iv, opdata.data()+START_IV, IV_LENGTH);
}

void BaseEntry::encrypt_opdata(const std::string &plaintext, const unsigned char iv[], const unsigned char key[], std::string &encoded_opdata) {
    string ciphertext;

    // Padding
    unsigned int padding_length = (BLOCK_LENGTH - plaintext.size() % BLOCK_LENGTH);
    AutoSeededRandomPool prng;

    byte* padding = new byte[padding_length];
    prng.GenerateBlock(padding, padding_length);

    // Encryption
    AES::Encryption aes_encryption(key, ENC_KEY_LENGTH);
    CBC_Mode_ExternalCipher::Encryption cbc_encryption(aes_encryption, iv);

    StreamTransformationFilter stf_encryptor(cbc_encryption, new StringSink(ciphertext), StreamTransformationFilter::NO_PADDING);

    string paddedtext = string(reinterpret_cast<const char *> (padding), padding_length) + plaintext;
    stf_encryptor.Put(reinterpret_cast<const byte *> (paddedtext.data()), paddedtext.size());
    stf_encryptor.MessageEnd();

    // opdata
    byte plaintext_length_byte[LENGTH_LENGTH];
    int64_t plaintext_length = static_cast<int64_t> (plaintext.size());
    memcpy(plaintext_length_byte, &plaintext_length, LENGTH_LENGTH);

    string opdata = string("opdata01") + string(reinterpret_cast<const char *> (plaintext_length_byte), LENGTH_LENGTH) + string(reinterpret_cast<const char *> (iv), IV_LENGTH) + ciphertext;

    // HMAC
    string mac;

    HMAC<SHA256> hmac(key+ENC_KEY_LENGTH, MAC_KEY_LENGTH);

    StringSource(opdata, true, new HashFilter(hmac, new StringSink(mac)));

    // Base64 encoding
    StringSource(opdata + mac, true, new Base64Encoder(new StringSink(encoded_opdata)));

    delete padding;
}

}
