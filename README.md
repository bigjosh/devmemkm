# devmemkm
A super-powered version of devmem/devmem2 that uses a kernel module to reach all addresses

## Overview

`devemem2` is handy, but there are addresses it can not reach since it runs in userspace. This version works just like the old one - except that it uses a Loadable Kernel Module to reach to any address that the kernel can reach.

## Usage

```
sudo insmod devmemkm.ko addr=0x44e10990 val=0x01
```

...where address gives the `unsigned` address to write to and val gives the `unsigned` value to write there.

If it worked, then you will get the message...
```
Error: could not insert module devmemkm.ko: Inappropriate ioctl for device
```
..and you will get an entry in the kernel log like this...
```
tail /var/log/kern.log
Dec 13 06:30:58 beaglebone kernel: [174144.222807] DMK: Address 0x44e10990 changed from 0x0 to 0x1
```

Note that the above error is good. We have to return an error so that the module is aborted and not left loaded in the system. This means you can `insmod` it repeatedly with no other effects besides writing to the specified addresses. 

If you forget to specify an address, then you will see..
```
root@beaglebone:~/devmemkm# sudo insmod devmemkm.ko
Error: could not insert module devmemkm.ko: Invalid parameters
```
This error means nothing happened. 

Example:

       devmemkm 0x44e10990 w 0x05


## Installation

1. Make sure you have [kernel header files installed](https://www.google.com/search?q=install+kernel+header+files&oq=install+kernel+header+files) and linked to `/lib/modules/$(shell uname -r)/build/`. 
1. `git clone https://github.com/bigjosh/devmemkm`
2. `cd devmemkm`
3. `make`


## Kernel header files

Yep this is harder than it should be. To compile a loadable kernel module, you need the header files that go with your specific kernel version.

On my BeagleBone running Debian, this worked...
```
apt-get update
sudo apt install linux-headers-$(uname -r)
```

I also had to add a link like this...
```
ln -s /usr/src/$(uname -r) /lib/modules/$(uname -r)/build
```

## My favorite use case

### Setting pinmode to PRU from a command prompt with no reboot!

When programming on the BeagleBone PRU, it is very nice to be able to twiddle an output pin trigger an oscilloscope or to see if some part of the code is even running. Normally to do this you'd need to make a device tree overlay, compile it, install it, reboot, and then kill yourself. 

Luckily with `devmemkm` we can configure the pin faster than you can say `SBBO`!

```
 sudo insmod devmemkm.ko addr=0x44e10990 val=0x05
```

That's it! Now pin `pr1_pru0_pru_r30_0` (or called `P9_31` or `GPIO_110` or `SPI1_SCLK` or `PIN 100` depending on where you grew up) is now assigned to bit0 on r30 on PRU #0. You can twiddle it with this lovely PRU ASM code...

```
// Turn on pin P9_31	
SET r30,r30, 0	

// DO IMPORTANT STUFF YOU WANT TO TIME

// Turn off pin P9_31
CLR r30,r30, 0
```

This works because `0x44e10990` is the address of the pin control register in the Control Module of the ARM chip, and `0x05` sets the 

How nice is that!?

## FAQ

> Why don't you just use normal `devemem2` to write to the address?

`devmem2` uses the `/dev/mem` interface so it can not access "kernel only" addresses. For example, if you try to access registers in the Control Module with `devmem2`, it just will not work. 

> Why don't you just use normal `/sys/class/` stuff?

As far as I can tell, it can not change the pinmux to PRU. 

> Why don't you just use normal device-tree stuff?

Because I deeply hate it, and I can not make myself waste another hour of my life figuring out problems with it. I have enough problems trying to get my own code to work.

Go ahead - configure PRU0 bit 0 pin to output on your board. Don't forget to copy the file to a location that changes depending on what version you are running. Don't forget that the [file *MUST* end](https://vadl.github.io/beagleboneblack/2016/07/29/setting-up-bbb-gpio#dtc-and-compiling-custom-overlays:~:text=Be%20sure%20to%20put%20the%20new,LOADED%20PROPERLY%20BY%20THE%20CAPE%20MANAGER!) in `-0A00`. Did you know that if the filename is longer than 14 chars then it will not work? (I do NOW). Don't call me when/if you finally get it working because I will already be dead.

Also as far as I can tell once you gotten a DTS just right and compiled it to a working DTD, you must reboot to change a pin to PRU mode that is not already set up for it. 

> Why not make a nice interface in `/sys` where I can write new addresses and values and lots of cool commands and stuff.

I'm sure that's how all the other ways of doing this started before they got complicated. Simple is best.

> Have you tried meditation? 

Listen I just wasted a fricken week to get something working that should have taken an hour - and all because of system bugs, bad design, wrong documentation, and other unnecessary friction. I just want to set my memory locations to what I want them to be and get on with my life. 

// Set P9_31 to PRU out. This is PRU reg 30 bit 0.


const unsigned gpio_set =  0x44e10990 ;

const unsigned pru_init_priority =  0x44e10608;

P9_29 GPIO3_15 LEDSCAPE42 SPI_1_DO GPIO_121 
control 44e109e4


