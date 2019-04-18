#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#define SL(str)    (str), (sizeof(str)-1)

typedef struct _binary {
    FILE *fp;
    int pos;
} Binary;

size_t file_size(FILE *fp) {
    size_t size;
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return size;
}

size_t file_write (FILE *fp, const void *data, size_t size) {
    return fwrite(data, sizeof(char), size, fp);
}

int binary_open(Binary *bin, const char *filename, const char *modes) {
    bin->fp = fopen(filename, modes);
    bin->pos = 0;
    if (bin->fp == NULL) {
        printf("open fail errno = %d reason = %s \n", errno, strerror(errno));
    }
    return bin->fp ? 1 : 0;
}

int binary_close(Binary *bin) {
    bin->pos = 0;
    return fclose(bin->fp);
}

int binary_setpos(Binary *bin, int pos) {
    int ret = fseek(bin->fp, pos, SEEK_SET);
    if (!ret) { // !0
        bin->pos = pos;
    }
    return ret;
}

char *binary_read(Binary *bin, int length) {
    char *result = calloc(length + 1, sizeof (char));
    int i = 0;
    int c;
    while (i < length && (c = fgetc(bin->fp)) != EOF) {
        result[i] = c;
        i++;
    }
    if (i != length) {
        free(result);
        binary_setpos(bin, bin->pos);
        return NULL;
    }
    bin->pos += i;
    return result;
}

int binary_skip(Binary *bin, int length) {
    if (length <= 0) {
        int i = 0;
        int c;
        while ((c = fgetc(bin->fp)) != EOF) {
            i++;
            if (c == '\0') {
                break;
            }
        }
        bin->pos += i;
        return 0;
    }
    int ret = fseek(bin->fp, length, SEEK_CUR);
    if (!ret) { // !0
        bin->pos += length;
    }
    return ret;
}

int binary_strpos(Binary *bin, const char *neele, size_t len) {
    int i = 0;
    int found = 0;
    int c;
    while (found < len && (c = fgetc(bin->fp)) != EOF) {
        if (c == neele[found]) {
            found++;
        } else {
            found = 0;
        }
        i++;
    }

    binary_setpos(bin, bin->pos);

    return found == len ? (bin->pos + i - len) : -1;
}

char *binary_readString(Binary *bin) {
    int length = 512;
    char *result = calloc(length, sizeof (char));
    int i = 0;
    int c;
    while ((c = fgetc(bin->fp)) != EOF) {
        if (i >= length) {
            char *ptr = NULL;
            length *= 2;
            ptr = realloc(result, length);
            if (!ptr) {
                return result;
            }
            result = ptr;
        }
        result[i] = c;
        i++;
        if (c == '\0') {
            break;
        }
    }
    bin->pos += i;

    return result;
}

uint32_t binary_readUint32(Binary *bin) {
    uint32_t value = 0;
    //bytes[0] + bytes[1]*256 + + bytes[2]*65536 + bytes[3]*16777216;
    int length = 4;
    int i = 0;
    int c;
    while (i < length && (c = fgetc(bin->fp)) != EOF) {
        value += (unsigned char) c << 8 * i;
        i++;
    }
    bin->pos += i;
    return value;
}

int binary_pos(Binary *bin) {
    return bin->pos;
}

int binary_eof(Binary *bin) {
    return feof(bin->fp);
}
