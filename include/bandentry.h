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

#include "baseentry.h"

namespace OPVault {

class BandEntry : BaseEntry {
    friend class Vault;
    friend class Band;

public:
    BandEntry() {
        updateState.update_d = false;
        updateState.update_o = false;
        updateState.update_plain = false;
    }

    std::string get_overview() { return o; }
    std::string get_uuid() { return uuid; }
    std::string get_category() { return category; }
    std::string get_data() { return d; }
    std::string get_folder() { return folder; }

    void set_overview(std::string _o) { o = _o; updateState.update_o = true; }
    void set_category(std::string _category) { category = _category; updateState.update_plain = true; }
    void set_data(std::string _d) { d = _d; updateState.update_d = true; }
    void set_folder(std::string _folder) { folder = _folder; updateState.update_plain = true; }

    void decrypt_data();
    void decrypt_overview();
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
        created(_created),
        o(_o),
        tx(_tx),
        updated(_updated),
        uuid(_uuid),
        category(_category),
        d(_d),
        folder(_folder),
        hmac(_hmac),
        k(_k)
    {
        updateState.update_d = false;
        updateState.update_o = false;
        updateState.update_plain = false;
    }

    int created;
    std::string o;
    int tx;
    int updated;
    std::string uuid;
    std::string category;
    std::string d;
    std::string folder;
    std::string hmac;
    std::string k;

    std::string decrypted_overview;
    std::string decrypted_data;

    UpdateState updateState;

    void verify_key();
    void decrypt_key(std::string &key);
    void init();
    void generate_hmac() {}
    void encrypt_data() {}
    void encrypt_overview() {}
};

}

#endif // BANDENTRY_H
