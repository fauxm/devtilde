# /dev/tilde v ~.2
A Linux kernel module that makes a character device that writes `~`.
It also makes a /dev/widetilde that prints `～`.


## Building:
Make sure you have kernel headers and shit installed and then just `make`.
`make clean` cleans the stuff.

## Installation:
`insmod ./devtilde.ko`.
`dmesg` should show a message: `~: Module loaded successfully~!`

## `udev` rule:
I've included a udev rule that sets the permissions of /dev/tilde to `0444`. You can toss it in `/etc/udev/rules.d` if you want to.
