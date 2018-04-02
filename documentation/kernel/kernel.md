## Kernel information

# Kernel type
This kernel is meant to be a monolithic kernel. This does not mean a pure monolithic kernel in which all drivers and such are loaded into the kernel code, but instead drivers and other similar things are loaded and run in kernel mode, but are dynamically loaded.
---
The kernel is loaded into the higher half of the memory, specifically the 3GB mark (0xC0000000).
---
The kernel, although it provides very simple drivers for applications to potentially use (a simple terminal driver, a simple serial driver, etc.), the kernel is not designed to use these drivers, but instead to use dynamically loaded drivers
