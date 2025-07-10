# shvfs - Self Healing Virtual File System (alpha/early idea only)
A virtual file system capable of detecting and fixing data corruption.

## How it should work
A [FUSE](https://github.com/libfuse/libfuse) virtual file system will be used to store alongside each file a Reed–Solomon ECC + SHA/BLAKE3 hash. Each time a file is opened, its correctness is checked and the extra header is stripped off. If the file is corrupted it's possible to automatically fix it.

It'd work similar to ZFS and BTRFS but in a virtual file system on top of a generic physical FS. Inspired by Cryptomator using Fuse3 to encrypt data on the disk but served in clear.

## If you like it, step in
I do not have enough skills to do that.
