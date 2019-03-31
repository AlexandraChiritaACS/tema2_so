#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "so_stdio.h"

#define BUFSIZE 4096
#define EOF (-1)
struct _so_file
{
    int fd;
    char* buf;
    int crpoz;
    int crbufsize;
};

SO_FILE *so_fopen(const char *pathname, const char *mode)
{
    SO_FILE *file = (SO_FILE *) malloc(sizeof(SO_FILE *));
    file->buf = (char *) malloc(sizeof(char) * BUFSIZE);
    file->crpoz = 0;
    file->crbufsize = BUFSIZE;

    if (strcmp(mode, "r") == 0)
    {
        file->fd = open(pathname, O_RDONLY, 0666);
    }

    if (strcmp(mode, "r+") == 0)
    {
        file->fd = open(pathname, O_RDWR, 0666);
    }

    if (strcmp(mode, "w") == 0)
    {
        file->fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    }

    if (strcmp(mode, "w+") == 0)
    {
        file->fd = open(pathname, O_RDWR | O_CREAT | O_TRUNC, 0666);
    }

    if (strcmp(mode, "a") == 0)
    {
        file->fd = open(pathname, O_WRONLY | O_CREAT | O_APPEND, 0666);
    }

    if (strcmp(mode, "a+") == 0)
    {
        file->fd = open(pathname, O_RDWR | O_CREAT | O_APPEND, 0666);
    }

    for (int i = 0; i < BUFSIZE; i++) {
        file->buf[i] = '\0';
    }

    return file;
}

int so_fclose(SO_FILE *stream)
{
    so_fflush(stream);
    int closeRes = close(stream->fd);
    
    if (!closeRes)
    {
        free(stream->buf);
        free(stream);
    }

    return closeRes;
}

int so_fileno(SO_FILE *stream)
{
    return stream->fd;
}

int so_fflush(SO_FILE *stream) {
    if (write(stream->fd, stream->buf, stream->crpoz) >= 0)
    {
        memset(stream->buf, 0, BUFSIZE);
        stream->crpoz = 0;
        stream->crbufsize = BUFSIZE;
        return 0;
    }

    return EOF;
}

int so_feof(SO_FILE *stream)
{
    if (read(stream->fd, &stream->buf + stream->crpoz, 1) <= 0)
    {
        return 1;
    }

    return 0;
}

int so_fputc(int c, SO_FILE *stream)
{
    stream->buf[stream->crpoz] = c;
    stream->crpoz++;

    if (c == (int) '\n' || stream->crpoz > BUFSIZE) 
    {
        if (so_fflush(stream) == EOF)
        {
            return EOF;
        }
    }

    return c;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream) {
    char * charPtr = (char *) ptr;
    int bytesToWrite = nmemb * size;
    for (int i = 0; i < bytesToWrite; i++)
    {
        if (so_fputc(*(charPtr + i), stream) == EOF)
        {
            return  i / size;    
        }
    }

    return size;
}

int so_fgetc(SO_FILE * stream)
{
    int readBytes = 0;

    if ((stream->crpoz == 0 && stream->buf[stream->crpoz] == '\0')
    || stream->crpoz == stream->crbufsize)
    {
        if ((readBytes = read(stream->fd, stream->buf, BUFSIZE)) > 0)
        {
            if (readBytes < stream->crbufsize) {
                stream->crbufsize = readBytes;
                stream->crpoz = 0;
            }
        } 
        else 
        {
            return EOF;            
        }
    }

    int res =  (int) stream->buf[stream->crpoz];
    stream->crpoz++;

    return res;
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream) {
    int bytesToRead = nmemb * size;

    for (int i = 0; i < bytesToRead; i++)
    {
        int charRead = so_fgetc(stream);

        if (charRead == EOF) {
            return i / size;
        }

        *((char *) ptr + i) = charRead;
    }

    return size;
}

int main() {
    SO_FILE *f;
    char* openMode = (char *) malloc(1 * sizeof(char));
    char *buf = (char *)malloc(14 * sizeof(char));
    memcpy(buf, "razvan", 6);
    openMode[0] = 'w';

    f = so_fopen("test.txt", openMode);

    //so_fputc('r', f);
    //so_fputc('\n', f);
    //int bytesRead = so_fread(buf, 1, 14, f);
    int bytesWrote = so_fwrite(buf, 1, 6, f);
    so_fclose(f);

    return 0;
}

