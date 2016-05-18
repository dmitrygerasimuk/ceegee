CeeGee
======

Small C project that I'm working on to teach myself the language.
This is designed for MS-DOS, meaning you need DJGPP to compile it.

This project is still in the very early stages and doesn't work yet!


Installation
------------

### Setting up DJGPP

To compile the engine, you'll need to have DJGPP set up to be able to
compile for MS-DOS. You can either get a pre-built release or build
the whole thing from source—follow the instructions on
[andrewwutw/build-djgpp](https://github.com/andrewwutw/build-djgpp) to get
a working compiler. If you follow the instructions in the link, your
compiler should also work out of the box for building the Allegro library.

Set an environment variable `$DJGPP_CC` to the path of your DJGPP GCC binary.
For example, you could add the following to your `~/.bashrc` file:

    export DJGPP_CC=/usr/local/djgpp/bin/i586-pc-msdosdjgpp-gcc

### Vendor code

To download vendor-specific code, run the `vendor.sh` script. This will
clone several Git repositories into the `vendor/` directory, which it will
create if it doesn't exist.

Only the Allegro library needs to be compiled separately. It's a fork
from the 4.2.2 version with some tweaks to make it compile on modern systems.
See the [allegro-4.2.2-xc](https://github.com/msikma/allegro-4.2.2-xc) page
for instructions.

All other vendor code is automatically compiled when you run `make`.

### Compiling the project

After Allegro has been compiled, it should be as simple as running `make`.
This creates a distribution in the `dist/` directory.


Dependencies
------------

* [allegro-4.2.2-xc](https://github.com/msikma/allegro-4.2.2-xc)
* [argparse](https://github.com/Cofyc/argparse)
* [xorshift](https://github.com/msikma/xorshift)


Copyright
---------

Copyright © 2015-2016, [Michiel Sikma](mailto:michiel@sikma.org)

MIT License