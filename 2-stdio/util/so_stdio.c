#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "so_stdio.h"

#define BUFSIZE 4096

struct _so_file
{
    int fd;
    char buf[BUFSIZE];
    int crpoz = 0;
};

SO_FILE *so_fopen(const char *pathname, const char *mode)
{
    SO_FILE *file = (SO_FILE *)malloc(sizeof(SO_FILE *));

    if (!strcmp(mode, "r") == 0)
    {
        file->fd = open(pathname, O_RDONLY);
    }

    if (!strcmp(mode, "r+") == 0)
    {
        file->fd = open(pathname, O_RDWR);
    }

    if (!strcmp(mode, "w") == 0)
    {
        file->fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC);
    }

    if (!strcmp(mode, "w+") == 0)
    {
        file->fd = open(pathname, O_RDWR | O_CREAT | O_TRUNC);
    }

    if (!strcmp(mode, "a") == 0)
    {
        file->fd = open(pathname, O_WRONLY | O_CREAT | O_APPEND);
    }

    if (!strcmp(mode, "a+") == 0)
    {
        file->fd = open(pathname, O_RDWR | O_CREAT | O_APPEND);
    }

    for (int i = 0; i < BUFSIZE; i++) {
        file->buf[i] = '\0';
    }

    return file;
}

int so_fclose(SO_FILE *stream)
{
    int closeRes = close(stream->fd);
    
    if (!closeRes)
    {
        free(stream);
    }

    return closeRes;
}

int so_fflush(SO_FILE *stream) {
    if (write(stream->fd, stream->buf) >= 0)
    {
        memset(stream->buf, 0, BUFSIZE);
        stream->crpoz = 0;
        return 0;
    }

    return EOF;
}

int so_fputc(int c, SO_FILE *stream)
{
    stream->crpoz = c;
    stream->crpoz++;

    if (c == (int) '\n' || crpoz > BUFSIZE) 
    {
        if (so_fflush() == EOF)
        {
            return EOF;
        }
    }

    return c;
}

int so_feof(SO_FILE *stream)
{
    if (read(stream->fd, &stream->buf + stream->crpoz, 1) <= 0) {
        return 1;
    }

    return 0;
}




