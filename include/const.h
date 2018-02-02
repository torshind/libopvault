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

#ifndef CONST_H
#define CONST_H

#include <unordered_map>

#include <cryptopp/config.h>
#if (CRYPTOPP_VERSION >= 600) && (__cplusplus >= 201103L)
    using byte = CryptoPP::byte;
#else
    typedef unsigned char byte;
#endif

//constants declarations
const int HEADER_LENGTH = 8;
const int LENGTH_LENGTH = 8;
const int START_IV = 16;
const int IV_LENGTH = 16;
const int START_CIPHER = 32;
const int NON_CIPHER_LENGTH = 64;
const int KEY_LENGTH = 64;
const int ENC_KEY_LENGTH = 32;
const int MAC_KEY_LENGTH = 32;

const int ITEM_KEY_LENGTH = 64;
const int ITEM_K_LENGTH = 112;

const int BLOCK_LENGTH = 16;

const int BAND_NUM = 16;
const char BAND_INDEXES[BAND_NUM] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

const char DBFILE[] = "opvault.db";
const char SQL_CREATE_PROFILE[] = "CREATE TABLE Profile (" \
                                  "lastUpdatedBy TEXT NOT NULL," \
                                  "updatedAt     INT  NOT NULL," \
                                  "profileName   TEXT NOT NULL," \
                                  "salt          TEXT NOT NULL," \
                                  "passwordHint  TEXT NOT NULL," \
                                  "masterKey     TEXT NOT NULL," \
                                  "iterations    INT  NOT NULL," \
                                  "uuid          CHAR(32) PRIMARY KEY NOT NULL," \
                                  "overviewKey   TEXT NOT NULL," \
                                  "createdAt     INT  NOT NULL );";

const char SQL_INSERT_PROFILE_ITEM[] = "INSERT INTO Profile (lastUpdatedBy, updatedAt, profileName, salt, passwordHint, masterKey, iterations, uuid, overviewKey, createdAt) " \
                                       "VALUES ('%s', %ld, '%s', '%s', '%s', '%s', %u, '%s', '%s', %ld);";

const char SQL_CREATE_ITEMS[] = "CREATE TABLE Items (" \
                                "created  INT  NOT NULL," \
                                "o        TEXT NOT NULL," \
                                "tx       INT  NOT NULL," \
                                "updated  INT  NOT NULL," \
                                "uuid     CHAR(32) PRIMARY KEY NOT NULL," \
                                "category TEXT NOT NULL," \
                                "d        TEXT NOT NULL," \
                                "fave     INT  NOT NULL," \
                                "folder   TEXT NOT NULL," \
                                "hmac     TEXT NOT NULL," \
                                "k        TEXT NOT NULL," \
                                "trashed  INT  NOT NULL );";

const char SQL_REPLACE_ITEM[] = "INSERT OR REPLACE INTO Items (created, o, tx, updated, uuid, category, d, fave, folder, hmac, k, trashed) " \
                                "VALUES (%ld, '%s', %ld, %ld, '%s', '%s', '%s', %lu, '%s', '%s', '%s', %d);";

const char SQL_CREATE_FOLDERS[] = "CREATE TABLE Folders (" \
                                  "created  INT  NOT NULL," \
                                  "o        TEXT NOT NULL," \
                                  "tx       INT  NOT NULL," \
                                  "updated  INT  NOT NULL," \
                                  "uuid     CHAR(32) PRIMARY KEY NOT NULL );";

const char SQL_REPLACE_FOLDER[] = "INSERT OR REPLACE INTO Folders (created, o, tx, updated, uuid) " \
                                  "VALUES (%ld, '%s', %ld, %ld, '%s');";

const char SQL_SELECT_PROFILE[] = "SELECT * from Profile";
const char SQL_SELECT_FOLDERS[] = "SELECT * from Folders";
const char SQL_SELECT_ITEMS[] = "SELECT * from Items";
const char SQL_SELECT_ITEMS_FOLDER[] = "SELECT * from Items WHERE folder = '%s'";
const char SQL_SELECT_ITEMS_CATEGORY[] = "SELECT * from Items WHERE category = '%s'";

const std::unordered_map<std::string, std::string> CATEGORIES = { {"001", "Login"},
                                                                  {"002", "Credit Card"},
                                                                  {"003", "Secure Note"},
                                                                  {"004", "Identity"},
                                                                  {"005", "Password"},
                                                                  {"099", "Tombstone"},
                                                                  {"100", "Software License"},
                                                                  {"101", "Bank Account"},
                                                                  {"102", "Database"},
                                                                  {"103", "Driver License"},
                                                                  {"104", "Outdoor License"},
                                                                  {"105", "Membership"},
                                                                  {"106", "Passport"},
                                                                  {"107", "Rewards"},
                                                                  {"108", "SSN"},
                                                                  {"109", "Router"},
                                                                  {"110", "Server"},
                                                                  {"111", "Email"} };

#endif
