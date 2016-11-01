# Research Kernel - Simple bootable thing.

This is essentially a [Meaty Skeleton](http://wiki.osdev.org/Meaty_Skeleton) bootable on **i686**, other platforms may follow in the future.

It may transform from a Meaty Skeleton to a bootable basic kernel in the future.

### Building and a Test drive.

You need:
* ruby (1.9 or 2.0 or later should work)
* A Cross compiler. See also: [GCC_Cross-Compiler](http://wiki.osdev.org/GCC_Cross-Compiler#Prebuilt_Toolchains), [Prebuilt toolchains](http://newos.org/toolchains/)

```
ruby build-i686.rb
make
```

To test it you need:
* qemu (qemu-system-i386) (don't worry, it also works with i686 binaries)

`qemu-system-i386 -kernel kernel.i686`


### Goals:

#### to be *easy to port*

I call it *easy to port* in opposition to *portable*. For example, Linux is "*portable*" as it is ported onto many platforms and hardware architectures, but that doesn't mean, that it is easy to port to a new architecture. The process to port Linux usually involves thousands of manhours, and bears many pitfalls, that are not obvious instantly.

In contrast, *easy to port* means, that it only supports a few architectures (or just one) but is carefully designed to be ported to other architectures later, and with minimal efforts.

