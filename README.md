# Introduction
Welcome to the shakey-table GitHub page, developed by Adam Englebright, Torin Storkey, Gennady Magin, and Jambulingam Kothandapani.
As part of the Real Time Embedded Programming 5 course at the University of Glasgow,
we have been tasked with developing a project that requires real time programming under GNU/Linux on a Raspberry Pi.
The project we have decided on is a self-balancing cup holder, implemented using a reaction wheel.
Essentially, an inverted pendulum. This git repository contains the code required to run this
self-balancing cup holder and instructions on how to build it for both software and hardware.
We used a Pi 5, which introduced some extra challenges as its GPIO differs greatly from
previous versions, meaning that common libraries like `pigpio` would not work.
This project was only tested on a Pi 5 using the latest Raspberry Pi OS
(`uname -a` output: *Linux rpi5-2 6.6.21-v8-16k+ #1 SMP PREEMPT Tue Mar 12 11:01:10 UTC 2024 aarch64 GNU/Linux*).
We cannot guarantee that it will work on other versions of the Pi or on other operating systems
or OS versions.

# Software
## Prerequisites/Dependencies
The following Debian packages (Raspberry Pi OS is built on Debian and has access to Debian packages) are required for this program to build and run on a rpi5:
* `linux-libc-dev`
* `libi2c-dev`
* `git`
* `make`
* `cmake`

These packages can be installed using `sudo apt install <package name>`.

There are also installs required that cannot be found in the standard repos, primarily due to the use of a Pi 5:
* `libgpiod` v2 is required. However, the version on the Debian Bookworm repo as of 2024-03-03 is v1.6.3 (despite the package name being `libgpiod2`).
This must be installed from source. `git clone git://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git`
([repo link](https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/)) and follow the `README` instructions.
The advised options to pass to the `autogen.sh` script are `--enable-tools=yes` (probably not necessary unless you want the command line tools)
and `--enable-bindings-cxx` (since this is a C++ program). Note that to build with C++ bindings, the `autoconf-archive` package is required
(this can be easily `apt install`'d from the Debian repos). You may also need to `apt install` the `libtool` package for the build to work.
* As of the latest kernel on 2024-04-03 (6.6.21-v8-16k+) on the Pi 5, this patch is required for PWM to work through the sysfs interface:
[https://github.com/raspberrypi/linux/pull/6026](https://github.com/raspberrypi/linux/pull/6026). Install using `sudo rpi-update pulls/6026`.
See [https://forums.raspberrypi.com/viewtopic.php?t=359251](https://forums.raspberrypi.com/viewtopic.php?t=359251),
particularly the [post by neuralassembly](https://forums.raspberrypi.com/viewtopic.php?p=2202349#p2202349) and onwards.

## Building the Software
To build the software, follow these instructions:
* `git clone https://github.com/embeddedgyro/shakey-table.git`
* `cd shakey-table`
* `mkdir build` (It is advisable to create an *out-of-source* build, hence this and the following step.)
* `cd build`
* `cmake ..`
* `make`

After this, an executable program called **ShakeyTable** should exist under the `build/src/` directory.
That is, `build/src/ShakeyTable`. This program can be run by entering either the absolute or relative
path of the executable. For example, `/some/path/from/root/shakey-table/build/src/ShakeyTable`, or
`src/ShakeyTable` if you are still in the `build` directory. If you are in the same directory as the
executable, you can enter `./ShakeyTable` (the leading dot means *this directory*).
