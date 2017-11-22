**libopvault**

C++ library to read/write 1Password database in OPVault format.

1Password is a password manager by AgileBits: https://1password.com/

I'm not affiliated with AgileBits. I wrote this software for personal use since
AgileBits doesn't provide a Linux implementation of 1Password. The library
design is based solely on the following information kindly provided by
AgileBits: https://support.1password.com/opvault-design/

Dependencies
------------

This project depends on the following third-party libraries:

* cryptopp: https://www.cryptopp.com/
* JSON for Modern C++: https://nlohmann.github.io/json/
* SQLite: https://www.sqlite.org/
* libuuid: http://e2fsprogs.sourceforge.net/

A sample OPVault file to run the test application is provided by AgileBits:
https://cache.agilebits.com/security-kb/

License
-------

This work is licensed under the MIT license. The full contents of this license
are available as the file `LICENSE.txt`.
The third party libraries are distributed under their own terms (see
LICENSE-3RD-PARTY.txt).

