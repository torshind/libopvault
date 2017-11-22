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

#ifndef BASEENTRY_H
#define BASEENTRY_H

#include <string>
#include <cryptopp/secblock.h>

namespace OPVault {

class BaseEntry
{
protected:
    BaseEntry() {}

    static CryptoPP::SecByteBlock derived_key;
    static CryptoPP::SecByteBlock overview_key;
    static CryptoPP::SecByteBlock master_key;

    void verify_opdata(const std::string &encoded_opdata, const CryptoPP::SecByteBlock &key);
    void decrypt_opdata(const std::string &encoded_opdata, const CryptoPP::SecByteBlock &key, std::string &plaintext);
    void get_iv(const std::string &encoded_opdata, CryptoPP::SecByteBlock &iv);
    void encrypt_opdata(const std::string &plaintext, const CryptoPP::SecByteBlock &iv, const CryptoPP::SecByteBlock &key, std::string &encoded_opdata);
};

}

#endif // BASEENTRY_H
