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

#ifndef PROFILEENTRY_H
#define PROFILEENTRY_H

#include "baseentry.h"

namespace OPVault {

class ProfileEntry : public BaseEntry
{
    friend class Vault;
    friend class Profile;

public:
    ProfileEntry() {}

    ProfileEntry(std::string _lastUpdatedBy,
                 int _updatedAt,
                 std::string _profileName,
                 std::string _salt,
                 std::string _passwordHint,
                 std::string _masterKey,
                 int _iterations,
                 std::string _uuid,
                 std::string _overviewKey,
                 int _createdAt) :
        lastUpdatedBy(_lastUpdatedBy),
        updatedAt(_updatedAt),
        profileName(_profileName),
        salt(_salt),
        passwordHint(_passwordHint),
        masterKey(_masterKey),
        iterations(_iterations),
        uuid(_uuid),
        overviewKey(_overviewKey),
        createdAt(_createdAt)
    {}

    void derive_keys(const std::string &master_password);
    void get_master_key();
    void get_overview_key();

private:
    std::string lastUpdatedBy;
    int updatedAt;
    std::string profileName;
    std::string salt;
    std::string passwordHint;
    std::string masterKey;
    unsigned int iterations;
    std::string uuid;
    std::string overviewKey;
    int createdAt;

    void get_profile_key(const std::string &encoded_key_opdata, CryptoPP::SecByteBlock &profile_key);
};

}

#endif // PROFILEENTRY_H
