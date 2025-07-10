// Self-healing virtual file system
#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define FILE_NAME "myfile.txt"
#define APPEND_TEXT "checksum_here"

static char *file_data = NULL;
static size_t file_size = 1024;

// Get file attributes (metadata).
// path: the path the kernel is requesting info about 
// stbuf: struct to fill
// fi: extra info
static int myfs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    memset(stbuf, 0, sizeof(struct stat)); // Set all bytes to zero so we're sure we don't return junk

    /*
        https://www.man7.org/linux/man-pages/man7/inode.7.html
        S_IFSOCK   0140000   socket
        S_IFLNK    0120000   symbolic link
        S_IFREG    0100000   regular file
        S_IFBLK    0060000   block device
        S_IFDIR    0040000   directory
        S_IFCHR    0020000   character device
        S_IFIFO    0010000   FIFO
    */

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else if (strcmp(path, "/" FILE_NAME) == 0) {
        stbuf->st_mode = S_IFREG | 0644;
        stbuf->st_nlink = 1;
        stbuf->st_size = file_size;
    } else {
        return -ENOENT;
    }
    return 0;
}

static int myfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    filler(buf, FILE_NAME, NULL, 0, 0);
    return 0;
}

static int myfs_open(const char *path, struct fuse_file_info *fi) {
    if (strcmp(path, "/" FILE_NAME) != 0)
        return -ENOENT;
    return 0;
}

static int myfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    if (strcmp(path, "/" FILE_NAME) != 0)
        return -ENOENT;

    if (offset < file_size) {
        if (offset + size > file_size)
            size = file_size - offset;
        memcpy(buf, file_data + offset, size);
    } else {
        size = 0;
    }

    return size;
}

static int myfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    if (strcmp(path, "/" FILE_NAME) != 0)
        return -ENOENT;

    size_t new_size = offset + size + strlen(APPEND_TEXT);
    char *new_data = realloc(file_data, new_size);
    if (!new_data)
        return -ENOMEM;

    file_data = new_data;
    memcpy(file_data + offset, buf, size);
    memcpy(file_data + offset + size, APPEND_TEXT, strlen(APPEND_TEXT));
    file_size = new_size;

    return size;  // Report only original write size
}

static const struct fuse_operations myfs_oper = {
    .getattr = myfs_getattr,
    .readdir = myfs_readdir,
    .open    = myfs_open,
    .read    = myfs_read,
    .write   = myfs_write,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &myfs_oper, NULL);
}
