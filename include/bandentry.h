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

#ifndef BANDENTRY_H
#define BANDENTRY_H

#include <vector>

#include "userentry.h"

namespace OPVault {

class BandEntry : public UserEntry {
    friend class Vault;
    friend class Band;

public:
    BandEntry() {
        updateState = false;
    }

    std::string get_category() { return category; }
    std::string get_folder() { return folder; }

    void set_category(std::string _category) {}
    void set_data(std::string _d);
    void set_folder(std::string _folder) {}

    void decrypt_data(std::string& data);
    void verify();

private:
    BandEntry(int _created,
              std::string _o,
              int _tx,
              int _updated,
              std::string _uuid,
              std::string _category,
              std::string _d,
              std::string _folder,
              std::string _hmac,
              std::string _k) :
        UserEntry(_created,
                  _o,
                  _tx,
                  _updated,
                  _uuid),
        category(_category),
        d(_d),
        folder(_folder),
        hmac(_hmac),
        k(_k)
    {
        updateState = false;
    }

    std::string category;
    std::string d;
    std::string folder;
    std::string hmac;
    std::string k;

    void verify_key();
    void decrypt_key(CryptoPP::SecByteBlock &key);
    void init();
    void generate_hmac() {}
};

}

#endif // BANDENTRY_H
