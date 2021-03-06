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

#pragma once

#include "baseitem.h"

namespace OPVault {

class ProfileItem : public BaseItem
{
    friend class Vault;
    friend class Profile;

public:
    ProfileItem() {}

    void derive_keys(const std::string &master_password);
    void get_master_key();
    void get_overview_key();

private:
    ProfileItem(std::string _lastUpdatedBy,
                long _updatedAt,
                std::string _profileName,
                std::string _salt,
                std::string _passwordHint,
                std::string _masterKey,
                unsigned int _iterations,
                std::string _uuid,
                std::string _overviewKey,
                long _createdAt) :
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

    std::string lastUpdatedBy;
    long updatedAt;
    std::string profileName;
    std::string salt;
    std::string passwordHint;
    std::string masterKey;
    unsigned int iterations;
    std::string uuid;
    std::string overviewKey;
    long createdAt;

    void get_profile_key(const std::string &encoded_key_opdata, CryptoPP::SecByteBlock &profile_key);
};

}
