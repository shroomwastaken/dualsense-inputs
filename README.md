# dualsense-inputs
outputs dualsense controller input data to terminal. dualsense has to be connected through USB (maybe i'll add bluetooth in the future)

i made this just to experiment with hidraw stuff.

this is only usable on linux.

## usage
`dualsense-inputs /dev/hidrawX` where X is a number.

you can find out the correct number by doing
```
$ sudo dmesg | grep playstation
```
after plugging in your controller

you'll see something along the lines of
```
[ 1432.167032] playstation 0003:054C:0CE6.0009: hidraw8: USB HID v1.11 Gamepad [Sony Interactive Entertainment DualSense Wireless Controller] on usb-0000:00:14.0-5/input3
```
in this example X = 8.

## output example
```
lx, ly = (129, 125) rx, ry = (127, 128) l2, r2 = (255, 0) buttons: 0418 ( square (dpad: center) l2 )
```

## resources used
linux hid_playstation driver: https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/drivers/hid/hid-playstation.c

game controller collective wiki page for dualsense: https://controllers.fandom.com/wiki/Sony_DualSense