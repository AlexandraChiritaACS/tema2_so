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
    int file_descriptor = -1;

    if (strcmp(mode, "r") == 0)
    {
        file_descriptor = open(pathname, O_RDONLY, 0666);
    }

    if (strcmp(mode, "r+") == 0)
    {
        file_descriptor = open(pathname, O_RDWR, 0666);
    }

    if (strcmp(mode, "w") == 0)
    {
        file_descriptor = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    }

    if (strcmp(mode, "w+") == 0)
    {
        file_descriptor = open(pathname, O_RDWR | O_CREAT | O_TRUNC, 0666);
    }

    if (strcmp(mode, "a") == 0)
    {
        file_descriptor = open(pathname, O_WRONLY | O_CREAT | O_APPEND, 0666);
    }

    if (strcmp(mode, "a+") == 0)
    {
        file_descriptor = open(pathname, O_RDWR | O_CREAT | O_APPEND, 0666);
    }

    if (file_descriptor < 0) {
        return NULL;
    }

    SO_FILE *file = (SO_FILE *)malloc(sizeof(SO_FILE));
    file->buf = (char *) malloc(sizeof(char) * BUFSIZE);

    memset(file->buf, 0, BUFSIZE);

    file->crpoz = 0;
    file->crbufsize = BUFSIZE;
    file->fd = file_descriptor;

    return file;
}

int so_fclose(SO_FILE *stream)
{
    so_fflush(stream);
    int closeRes = close(stream->fd);
    
    free(stream->buf);
    free(stream);

    return closeRes;
}

int so_fileno(SO_FILE *stream)
{
    return stream->fd;
}

int so_fflush(SO_FILE *stream) {
    if (stream != NULL) {
        if (write(stream->fd, stream->buf, stream->crpoz) >= 0)
        {
            memset(stream->buf, 0, BUFSIZE);
            stream->crpoz = 0;
            stream->crbufsize = BUFSIZE;
            return 0;
        }
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
    if (stream != NULL) {
        if (stream->crpoz >= stream->crbufsize) 
        {
            if (so_fflush(stream) == EOF)
            {
                return EOF;
            }
        }

        stream->buf[stream->crpoz] = c;
        stream->crpoz++;
        return c;
    }

    return EOF;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream) {
    char * charPtr = (int *) ptr;
    int bytesToWrite = nmemb * size;

    for (int i = 0; i < bytesToWrite; i++)
    {
        if (so_fputc(*(charPtr + i), stream) == EOF && *(charPtr + i) != EOF)
        {
            return  i / size;    
        }
    }

    return nmemb / size;
}

int so_fgetc(SO_FILE * stream)
{
    int readBytes = 0;

    if ((stream->crpoz == 0 && stream->buf[stream->crpoz] == '\0')
    || stream->crpoz == stream->crbufsize)
    {
        if ((readBytes = read(stream->fd, stream->buf, BUFSIZE)) > 0)
        {
            stream->crbufsize = readBytes;
            stream->crpoz = 0;
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
        *((char *) ptr + i) = charRead;
    }

    return nmemb / size;
}