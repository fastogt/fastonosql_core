About FastoNoSQL core
===============

FastoGT common sources

Visit our site: [www.fastogt.com](http://www.fastogt.com)

Contribute
==========
Contributions are always welcome! Just try to follow our coding style: [FastoGT Coding Style](https://github.com/fastogt/fastonosql/wiki/Coding-Style)

Build
========

Windows
-------
`cmake .. -GNinja -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=/mingw64`<br>
`cmake .. -GNinja -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=/mingw32`

Linux, FreeBSD, MacOS X
-------
`cmake .. -GNinja -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=/usr/local`

Android
-------
`cmake .. -DCMAKE_STRIP="/opt/android-ndk/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-strip" -DCMAKE_AR="/opt/android-ndk/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-ar" -DCMAKE_C_COMPILER="/opt/android-ndk/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-gcc" -DCMAKE_CXX_COMPILER="/opt/android-ndk/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-g++" -GNinja -DCMAKE_TOOLCHAIN_FILE=../cmake/android.toolchain.cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=/opt/android-ndk/platforms/android-9/arch-arm/usr/`

License
=======

Copyright (C) 2014-2018 FastoGT (http://www.fastogt.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 3 as 
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
