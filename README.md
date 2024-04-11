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

# Hardware

## Components

The following components were used for this project:#

* Raspberry Pi 5 - Microcontroller. Documentation for it can be found here [https://www.raspberrypi.com/documentation/](https://www.raspberrypi.com/documentation/).

* Current Sensor - INA260. Used for reading current flowing through the motor for PID motor control. Uses I2C communication protocol for interfacing with Pi. Datasheet can be found here [https://www.ti.com/lit/ds/symlink/ina260.pdf?ts=1712654268303&ref_url=https%253A%252F%252Fwww.google.com%252F](https://www.ti.com/lit/ds/symlink/ina260.pdf?ts=1712654268303&ref_url=https%253A%252F%252Fwww.google.com%252F).
Should be connected in series with the motor with Vin+ terminal connected to V- on the motor and Vin- connected to the junciton between inductors and capacitors of the LC filter. Is powered by a 3V3 supply from the Pi. Communicates with the Pi through I2C1 pins (SDA->gpio27, SCL->gpio28) which don't have internal pull-up resistors. Alert pin is connected to gpio5 on the Pi. Additional three 10KOhm resistors should be connected between the SCL, SDA and Alert lines and 3V3 to eliminate possibility of reading undefined state during swithching of the signal on the lines.
ADD INFO ABOUT HOW IT FUNCTIONS

* Gyro-Acelerometer Sensor - MPU6050. Measures The incine of the table from up-right position and captures the acceleration of the table in a 2D plane. Measurements are used for PID motor control. Uses I2C communication protocol for interfacing with Pi. Datasheet can be found here [https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/2204/SEN0142_Web.pdf](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/2204/SEN0142_Web.pdf).
Should be mounted firmly to the surface of the table with connection pins facing the ground. INSERT IMAGE OF THE ORIENTATION OR DESCRITE IT PROPERLY. Is powered by a 3V3 supply from the Pi. It is communicating with the Pi through I2C0 pins (SDA->gpio2, SCL->gpio3) which have internal pull-up resistors. No extra external cirucitry is needed. Interrup pin should be connected to gpio4.
DESCRIBE FUNCTIONALITY IN MORE DETAIL

* Motor Driver - Cytron MD20A. Regulates voltage supplied to the motor, controls the direction and speed of motor rotation with PWM. Datasheet can be found here [https://robu.in/wp-content/uploads/2019/05/MD20A-Datasheet.pdf](https://robu.in/wp-content/uploads/2019/05/MD20A-Datasheet.pdf).
Uses 12V external DC power supply connected to VA+ and VB- for delivering power to the motor. In this project an external power generator was used for this purpose. Use thick multicore wires with good isolation for power lines.
Controls the motor rotation by recieving a 20kHz PWM signal with a PID-controlled Duty Cycle from gpio18 on the Pi. Direction is controlled by a High-Low output from gpio23.

* Motor - 12V DC Motor 12kg-cm 350RPM w/Encoder [https://uk.robotshop.com/products/12v-dc-motor-12kg-cm-350rpm-encoder](https://uk.robotshop.com/products/12v-dc-motor-12kg-cm-350rpm-encoder).
Used for rotating the reaction wheel and create the nesessary torque for table stabilisation. V+ terminal should be connected to MA terminal on the motor driver, and V- terminal connected to Vin+ terminal on the current sensor.

* Low-Pass Filter - LC filter with a cut-off at about 1200Hz to eliminate high-frequency noise coming from the motor driver and Pi PWM signal. 
It uses two 560uH choke inductors(`MCAP115018077A-561LU`) after MB terminal on the motor driver in series with the motor and the current sensor. Seven 2.2uF polyester film capacitors(`7212-707`) after inductors in parallel around the current sensor and motor driver. At high frequencies inductor becomes very resistive and blocks high freqeuncy signals from passing into the circuit. Capacitor becomes a short circutit for high frequency signals. High freqeuncies flow through it, avoiding the current sensor and the motor. Inductors with high current rating above 10A and capacitos with high voltage rating above 20V should be used in this project to prevent damage and burning of the filter. Polarised capacitors cannot be used, as current can flow in both directions in this circuit.

## Hardware Assembly

INSERT IMAGE WITH FULL SETUP (REAL WORLD AND SCHEMATIC)
INSERT IMAGE WITH CORRECT CONFIGURATION OF MPU6050
INSERT IMAGE WITH PI5 PIN LABLES

## CAD Design

# Software
## Prerequisites/Dependencies
The following Debian packages (Raspberry Pi OS is built on Debian and has access to Debian packages) are required for this program to build and run on a rpi5:
* `linux-libc-dev`
* `libi2c-dev`
* `make`
* `cmake`
* `g++`

The following packages are recommended:
* `git` for cloning the repository, although you can simply download the code from the GitHub website if you wish.
* `doxygen` for creating nice documentation from the comments in the code.
* `texlive-full` for compiling a PDF document from the LaTeX code created by Doxygen (which also creates a website that you can view in your browser,
so this is not necessary to view the documentation).

These packages can be installed using `sudo apt install <package name>`.

There are also installs required that cannot be found in the standard repos, primarily due to the use of a Pi 5:
* `libgpiod` v2 is required. However, the version on the Debian Bookworm repo as of 2024-03-03 is v1.6.3 (despite the package name being `libgpiod2`).
This must be installed from source. `git clone git://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git`
([repo link](https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/)) in a suitable place and follow the `README` instructions.
The advised options to pass to the `autogen.sh` script are `--enable-tools=yes` (not *necessary*, but provides useful command line tools)
and `--enable-bindings-cxx` (**necessary**, since this is a C++ program). Note that to build with C++ bindings, the `autoconf-archive` package is required
(this can be easily `apt install`'d from the Debian repos). You may also need to `apt install` the `libtool` package for the build to work.
* As of the latest kernel for the Pi 5 on 2024-04-03 (6.6.21-v8-16k+), this patch is required for PWM to work through the sysfs interface:
[https://github.com/raspberrypi/linux/pull/6026](https://github.com/raspberrypi/linux/pull/6026). Install using `sudo rpi-update pulls/6026`.
See [https://forums.raspberrypi.com/viewtopic.php?t=359251](https://forums.raspberrypi.com/viewtopic.php?t=359251),
particularly the [post by neuralassembly](https://forums.raspberrypi.com/viewtopic.php?p=2202349#p2202349) and onward.

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

## Documentation
Documentation of this project is provided by Doxygen formatted comments in the code.
Doxygen can be used to create a website and PDF document that organise the documentation
in a way that is clear, easy to navigate, and easy to read.

### Website
To create the documentation website, run `make doc` after the `cmake ..` command from the above build instructions.
Running `cmake ..` will create a Doxyfile, and running `make doc` will run Doxygen on this file to generate
the documentation website under the `build/html/` directory.

To view this website, open your browser of choice.
In the URL bar, enter `file:///some/path/from/root/shakey-table/build/html/index.html`. This will open the
main page of the website. From there you can find the rest of the documentation using the navigation
tools at the top of the page.

### PDF Document
After running `make doc`, there will also be a `build/latex/` directory. If you have a TeX distribution
installed, you can `cd` into this directory and enter `make`. This will invoke `pdflatex` to generate
a PDF document called `refman.pdf`, which will contain the Doxygen generated documentation for the project.
View with your PDF reader of choice. Most browsers can also view PDF documents.
