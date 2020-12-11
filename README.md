# devmemkm
A super-powered version of devmem/devmem2 that uses a kernel module to reach all addresses

## Overview

`devemem2` is handy, but there are addresses it can not reach since it runs in userspace. This version works just like the old one - except that it uses a Loadable Kernel Module to reach to any address that the kernel can reach.

## Syntax

Compatible style:

```
devmemkm address [type [data]]

       address - Memory address to act upon.

       type    - Access operation type: [b]yte, [h]alfword, [w]ord. Defaults to word if ommited.  

       data    - Data to be written. Address is read if ommited.
       
Example:

       devmemkm 0x44e10990 w 0x05
```

## Installation

1. Make sure you have [kernel header files installed](https://www.google.com/search?q=install+kernel+header+files&oq=install+kernel+header+files). 
1. `git clone https://github.com/bigjosh/devmemkm`
2. `cd devmemkm`
3. `make`
4. `make install`



