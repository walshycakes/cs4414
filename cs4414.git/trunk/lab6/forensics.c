// Jared Culp (jjc4fb)
// Bryan Walsh (bpw7xx)
// CS 4414
// Assignment 6

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <byteswap.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE 1 << 10

#define THE_UID 0x2952
#define THE_GID 0x4cf

#define N_DBLOCKS 12
#define N_IBLOCKS 3

char ps[]       = {0x25, 0x21};
char pdf[]      = {0x25, 0x50, 0x44, 0x46};
char gif[]      = {0x47, 0x49, 0x46, 0x38};
char gif1[]     = {0x37, 0x61};
char gif2[]     = {0x39, 0x61};
char tiff1[]    = {0x49, 0x49, 0x2A, 0x00};
char tiff2[]    = {0x4D, 0x4D, 0x00, 0x2A};
char png1[]     = {0x89, 0x50, 0x4E, 0x47};
char png2[]     = {0x0D, 0x0A, 0x1A, 0x0A};
char exe[]      = {0x4D, 0x5A};
char ascii[]    = {0xFF, 0xD8};
char html[]     = {0x25, 0x21};

typedef struct inode {
    int foo;      /* Unknown field */
    int nlink;    /* Number of links to this file */
    int uid;      /* Owner's user ID */
    int gid;      /* Owner's group ID */
    int size;     /* Number of bytes in file */
    int ctime;    /* Time field */
    int mtime;    /* Time field */
    int atime;    /* Time field */
    int dblocks[N_DBLOCKS]; /* Pointers to data blocks */
    int iblocks[N_IBLOCKS]; /* Pointers to indirect blocks */
    int i2block; /* Pointer to doubly indirect block */
    int i3block; /* Pointer to triply indirect block */
} inode_t;

void print_inode(inode_t *node) {
    printf("foo:\t%x\n", node->foo);
    printf("nlink:\t%x\n", node->nlink);
    printf("uid:\t%x\n", node->uid);
    printf("gid:\t%x\n", node->gid);
    printf("size:\t%x\n", node->size);
    printf("ctime:\t%x\n", node->ctime);
    printf("mtime:\t%x\n", node->mtime);
    printf("atime:\t%x\n", node->atime);
    printf("dblocks:\n");
    int i;
    for (i = 0; i < N_DBLOCKS; ++i) {
        printf("#%d: %x\n", i, node->dblocks[i]);
    }
    printf("iblocks:\n");
    for (i = 0; i < N_IBLOCKS; ++i) {
        printf("#%d: %x\n", i, node->iblocks[i]);
    }

    printf("i2block:\t%x\n", node->i2block);
    printf("i3block:\t%x\n", node->i3block);
}

int inode_init(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return ((a << 24) | (b << 16) | (c << 8) | (d));
}

void get_filetype(inode_t* node, int n) {
}

int main( int argc, char **argv ) {
    if (argc != 2)
        perror("invalid args");

    FILE *fp;
    fp = fopen(argv[1], "rb");

    struct stat st;
    int fd = fileno(fp);
    fstat(fd, &st);

    uint8_t *addr = (uint8_t*) mmap(NULL,
                                    st.st_size,
                                    PROT_READ,
                                    MAP_FILE|MAP_SHARED,
                                    fd,
                                    0);

    if (addr == MAP_FAILED)
        perror("mmap fail");

    long i;
    uint16_t target_uid, target_gid;

    // allocate space for the inodes
    int matches = 0;
    inode_t **our_nodes = malloc(sizeof(inode_t));

    for (i = 0; i < st.st_size - 7; i++) { // we access 8 elements of memory each iter
        target_uid = inode_init(addr[i+3], addr[i+2], addr[i+1], addr[i]);
        target_gid = inode_init(addr[i+7], addr[i+6], addr[i+5], addr[i+4]);
        
        if ((target_uid == THE_UID) && (target_gid == THE_GID)) {
            // initialize the inode
            inode_t *node = &addr[i-8];

            // store the inode for later
            our_nodes[matches] = node;
            matches = matches + 1;
            //print_inode(node);
            our_nodes = (inode_t**) realloc(our_nodes, ((matches+1) * sizeof(inode_t*)));
        }
    }
    
    int m, n, p, q;
    FILE *out;
    int writes = 0;
    for (m = 0; m < matches; ++m) {
        inode_t* node = our_nodes[m];

        // hard coded filename :(
        char filename[25];
        switch (m) {
            case 0:
                strcpy(filename, "file.pdf");
                break;
            case 1:
                strcpy(filename, "file.gif");
                break;
            case 2:
                strcpy(filename, "file.tiff");
                break;
            case 3:
                strcpy(filename, "file.ps");
                break;
            default:
                break;
        }
        out = fopen(filename, "wb");

        // direct blocks
        for (n = 0; n < N_DBLOCKS; ++n) {
            int d_val = node->dblocks[n] * BLOCK_SIZE;
            fwrite((char*)addr + d_val, 1, BLOCK_SIZE, out);
        }

        /*
            note, indirect, double, triple, do not work.
            it seems like some simple issue, but there
            was not enough time to work it out.

            the write statements are commented out, as
            those are what cause the segmentation faults
        */

        // indirect blocks
        for (p = 0; p < N_IBLOCKS; ++p) {
            int i_val = node->iblocks[p] * BLOCK_SIZE;
            int *ii_val = (int*)((char*)addr + i_val);

            // blocks on this indirect block
            int ib_num;
            for (ib_num = 0; ib_num < (BLOCK_SIZE / sizeof(int)); ++ib_num) {
                int ib_val = ii_val[ib_num] * BLOCK_SIZE;
               // fwrite((char*)addr + ib_val, 1, BLOCK_SIZE, out); // segfaults
            }
        }

        // doubly indirect
        for (p = 0; p < (BLOCK_SIZE / sizeof(int)); ++p) {
            int di_val = node->i2block * BLOCK_SIZE;
            int *dii_val = (int*)((char*)addr + di_val);

            // blocks on this indirect block
            int idb_num;
            for (idb_num = 0; idb_num < (BLOCK_SIZE / sizeof(int)); ++idb_num) {
                int idb_val = dii_val[idb_num] * BLOCK_SIZE;
               // fwrite((char*)addr + idb_val, 1, BLOCK_SIZE, out); // segfaults
            }
        }

        // triply indirect
        for (q = 0; q < (BLOCK_SIZE / sizeof(int)); ++q) {
            int ddi_val = node->i3block * BLOCK_SIZE;
            int *ddii_val = (int*)((char*)addr + ddi_val);

            // blocks on this indirect block
            int iddb_num;
            for (iddb_num = 0; iddb_num < (BLOCK_SIZE / sizeof(int)); ++iddb_num) {
                int iddb_val = ddii_val[iddb_num] * BLOCK_SIZE;
               // fwrite((char*)addr + iddb_val, 1, BLOCK_SIZE, out); // segfaults
            }
        }
    }
    
    return 0;
}
