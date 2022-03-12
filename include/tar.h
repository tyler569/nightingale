#pragma once

struct tar_header {
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
};

enum tar_typeflag {
    REGTYPE = '0',
    AREGTYPE = '\0',
    LNKTYPE = '1',
    CHRTYPE = '3',
    BLKTYPE = '4',
    DIRTYPE = '5',
    FIFOTYPE = '6',
    XATTR = 'x',
};
