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

struct BandEntry : BaseEntry {
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

    BandEntry() {}
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
    {}

    void decrypt_data();
    void decrypt_overview();

private:
    void decrypt_key(std::string &key);
};

}

#endif // BANDENTRY_H
