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

#include "useritem.h"

namespace OPVault {

class BandItem : public UserItem {
    friend class Vault;
    friend class Band;

public:
    BandItem() {
        fave = 0;
        folder = "NULL";
        trashed = -1;
        updateState = false;
    }

    std::string& get_category() { return category; }
    unsigned long get_fave() { return fave; }
    std::string& get_folder() { return folder; }
    int get_trashed() { return trashed; }

    void set_category(const std::string &_category);
    void set_data(const std::string &_d);
    void set_fave(const unsigned long _fave);
    void set_folder(const std::string &_folder);
    void set_trashed(const int _trashed);

    void decrypt_data(std::string& data);

private:
    BandItem(long _created,
             std::string _o,
             long _tx,
             long _updated,
             std::string _uuid,
             std::string _category,
             std::string _d,
             unsigned long _fave,
             std::string _folder,
             std::string _hmac,
             std::string _k,
             int _trashed) :
        UserItem(_created,
                 _o,
                 _tx,
                 _updated,
                 _uuid),
        category(_category),
        d(_d),
        fave(_fave),
        folder(_folder),
        hmac(_hmac),
        k(_k),
        trashed(_trashed)
    {
        updateState = false;
    }

    std::string category;
    std::string d;
    unsigned long fave;
    std::string folder;
    std::string hmac;
    std::string k;
    int trashed;

    std::string hmac_in_str();
    void verify();
    void verify_key();
    void decrypt_key(CryptoPP::SecByteBlock &key);
    void init();
    void generate_hmac();
};

}

#endif // BANDENTRY_H
