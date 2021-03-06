AeonGUI README (05-26-2013)
===========================

This file contains the following sections:

DESCRIPTION
HISTORY
COMPILATION
LICENSE
AUTHOR

DESCRIPTION
===========
AeonGUI is a graphic user interface library to be used primarily on video games and interactive media.
It's goals are to keep the rendering engine separated from widget logic to make things easier for
would be users (developers intending to use the library on their own product), 
renderer writers (developers looking to implement support for a particular graphic API) and
widget writers (developers looking to add new controls to the library).

WARNING!!!: The library is not currently on a stable or even usable state.

HISTORY
=======
The idea for the library was born around 2004 or 2005 under the name 'Glitch', and has evolved overtime but never quite reached maturity.
The name change to AeonGUI came about as a way to identify the library as part of the AeonGames brand
and to avoid confusion since originaly the name 'GLitch' was chosen to emphasize OpenGL support, which is still the priority, 
but not necesarily the only graphic API supported.

COMPILATION
===========
The library uses CMake to build the required files used to build the binary depending on the platform.
For information on how to use CMake refer to www.cmake.org.
There is only "official" support for Windows and Linux at this time, 
however, Linux builds may break from time to time as most development is done on Windows.

Optional USE variables are available to add support for various features such as freetype font rendering and PNG file format loading,
these add external dependencies to the library, so they are all initially set to OFF, if turned ON, the user has the option to provide
the dependencies or let CMake download and configure the dependencies which will show up as projects inside the build environment.
The rules to download and configure external dependencies are most useful to Windows users as Linux distributions usually provide 
development libraries for most software packages.

LICENSE
=======
The library is released under the terms of the permisive Apache 2.0 license (http://www.apache.org/licenses/LICENSE-2.0)
<Insert Objectivist Sermon Here>

The Aeon Games logo is NOT covered by the Apache license,
it is a Trade Mark and may not be used for any purpose without permision.

Some of the code and assets are not covered by the Apache license:

fonts     is Copyright (c) 2003 by Bitstream, Inc., refer to COPYRIGHT.TXT for license terms.

AUTHOR
======
The only author (for now) of AeonGUI is Rodrigo Hernandez and can be reached at kwizatz at aeongames dot com.
