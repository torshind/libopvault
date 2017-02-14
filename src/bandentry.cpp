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

#include "const.h"
#include "vault.h"

#include "bandentry.h"

using namespace std;
using namespace CryptoPP;

namespace OPVault {

void BandEntry::decrypt_key(std::string &key) {
    string encrypted_key;
    StringSource(k, true, new Base64Decoder(new StringSink(encrypted_key)));

    const byte* key_byte = (byte*) encrypted_key.data();

    byte iv[IV_LENGTH] = "";
    memcpy(iv, key_byte, IV_LENGTH*sizeof(byte));

    byte *ciphertext = NULL;
    ciphertext = (byte*) calloc(ITEM_KEY_LENGTH, sizeof(byte));
    memcpy(ciphertext, key_byte+IV_LENGTH, ITEM_KEY_LENGTH*sizeof(byte));

    byte hmac[HMAC_LENGTH] = "";
    memcpy(hmac, key_byte+ITEM_START_HMAC, HMAC_LENGTH*sizeof(byte));

    AES::Decryption aes_decryption(master_key, ENC_KEY_LENGTH);
    CBC_Mode_ExternalCipher::Decryption cbc_decryption(aes_decryption, iv);

    StreamTransformationFilter stf_decryptor(cbc_decryption, new StringSink(key), StreamTransformationFilter::NO_PADDING);

    stf_decryptor.Put((const byte *)ciphertext, ITEM_KEY_LENGTH);
    stf_decryptor.MessageEnd();
}

void BandEntry::decrypt_data() {
    string item_key;
    decrypt_key(item_key);

    decrypt_opdata(d, (unsigned char*) item_key.data(), decrypted_data);
}

void BandEntry::decrypt_overview() {
    decrypt_opdata(o, (unsigned char*) overview_key, decrypted_overview);
}

}
