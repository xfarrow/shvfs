# shvfs - Self Healing Virtual File System

> [!CAUTION]
> This is in early-idea stage. If you like the project, step in as the undersigned does not have enough skills to accomplish that

A virtual file system capable of detecting and fixing data corruption.

## How it works
A [Fuse](https://github.com/libfuse/libfuse) virtual file system will be used to store alongside each file Reed–Solomon error correcting codes and an hash. Each time a file is opened (or by performing a scrub), its correctness is checked and the extra header is stripped off before serving it to the user. If the file is corrupted it's possible to automatically fix it.

It'd work similar to ZFS and BTRFS but in a virtual file system on top of a generic physical FS. Inspired by Cryptomator using Fuse3 to encrypt data on the disk but served in clear.
