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

#include "const.h"
#include "dbg.h"

#include "baseitem.h"

using namespace CryptoPP;

namespace OPVault {

SecByteBlock BaseItem::derived_key(KEY_LENGTH);
SecByteBlock BaseItem::master_key(KEY_LENGTH);
SecByteBlock BaseItem::overview_key(KEY_LENGTH);

void BaseItem::verify_opdata(const std::string &encoded_opdata, const SecByteBlock &key) {
    std::string opdata;
    StringSource(encoded_opdata, true, new Base64Decoder(new StringSink(opdata)));

    HMAC<SHA256> hmac(key.data()+ENC_KEY_LENGTH, MAC_KEY_LENGTH);
    const int flags = HashVerificationFilter::THROW_EXCEPTION | HashVerificationFilter::HASH_AT_END;

    StringSource(opdata, true, new HashVerificationFilter(hmac, nullptr, flags));
}

void BaseItem::decrypt_opdata(const std::string &encoded_opdata, const SecByteBlock &key, std::string &plaintext) {
    try {
        verify_opdata(encoded_opdata, key);
    }
    catch (...) {
        throw std::invalid_argument("libopvault: failed hash check");
    }

    std::string opdata;
    StringSource(encoded_opdata, true, new Base64Decoder(new StringSink(opdata)));

    DBGVAR(opdata);

    size_t ciphertext_length;
    ciphertext_length = opdata.length() - NON_CIPHER_LENGTH;

    char header[HEADER_LENGTH];
    memcpy(header, opdata.data(), HEADER_LENGTH);

    if (memcmp(header, "opdata01", HEADER_LENGTH)) {
        throw std::invalid_argument("libopvault: invalid opdata value");
    }

    size_t plaintext_length;
    memcpy(&plaintext_length, opdata.data()+HEADER_LENGTH, LENGTH_LENGTH);

    SecByteBlock iv(reinterpret_cast<const unsigned char *> (opdata.data())+START_IV, AES::BLOCKSIZE);

    std::string ciphertext = std::string(opdata.data()+START_CIPHER, ciphertext_length);

    CBC_Mode<AES>::Decryption decryption(key, ENC_KEY_LENGTH, iv);

    if (!plaintext.empty()) {
        plaintext.clear();
    }
    StringSource(ciphertext, true, new StreamTransformationFilter(decryption, new StringSink(plaintext), StreamTransformationFilter::NO_PADDING));

    plaintext.erase(0, ciphertext_length-plaintext_length);

    DBGVAR(plaintext);
}

void BaseItem::encrypt_opdata(const std::string &plaintext, const SecByteBlock &iv, const SecByteBlock &key, std::string &encoded_opdata) {
    std::string ciphertext;

    // Padding
    unsigned int padding_length = (BLOCK_LENGTH - plaintext.size() % BLOCK_LENGTH);
    AutoSeededRandomPool prng;

    unsigned char* padding = new unsigned char[padding_length];
    prng.GenerateBlock(padding, padding_length);

    // Encryption
    CBC_Mode<AES>::Encryption encryption(key, ENC_KEY_LENGTH, iv);

    std::string paddedtext = std::string(reinterpret_cast<const char *> (padding), padding_length) + plaintext;
    StringSource(paddedtext, true, new StreamTransformationFilter(encryption, new StringSink(ciphertext), StreamTransformationFilter::NO_PADDING));

    // opdata
    char plaintext_length_str[LENGTH_LENGTH];
    size_t plaintext_length = plaintext.size();
    memcpy(plaintext_length_str, &plaintext_length, LENGTH_LENGTH);

    std::string opdata = std::string("opdata01") + std::string(plaintext_length_str, LENGTH_LENGTH) + std::string(reinterpret_cast<const char *> (iv.data()), AES::BLOCKSIZE) + ciphertext;

    // HMAC
    std::string mac;

    HMAC<SHA256> hmac(key.data()+ENC_KEY_LENGTH, MAC_KEY_LENGTH);

    StringSource(opdata, true, new HashFilter(hmac, new StringSink(mac)));

    // Base64 encoding
    StringSource(opdata + mac, true, new Base64Encoder(new StringSink(encoded_opdata), false));

    delete[] padding;
}

}
