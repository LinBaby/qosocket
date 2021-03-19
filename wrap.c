#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>

void perr_exit(const char *s)
{
    perror(s);
    exit(-1);
}

int Accept(int fd, struct sockaddr *sa, socklen_t *sa_len_ptr)
{
    int n;

again:
    if ( (n = accept(fd, sa, sa_len_ptr)) < 0 ) {
        if ( (errno == ECONNABORTED) || (errno == EINTR) ) {
             goto again;
        } else {
            perr_exit("Accept error");
        }
    }

    return n;
}

int Bind(int fd, const struct sockaddr *sa, socklen_t sa_len)
{
    int n;

    if ( (n = bind(fd, sa, sa_len)) < 0 ) {
        perr_exit("Bind error");
    }

    return n;
}

int Listen(int fd, int backlog)
{
    int n;

    if ( (n = listen(fd, backlog)) < 0 ) {
        perr_exit("Listen error");
    }

    return n;
}

int Socket(int family, int type, int protocol)
{
    int n;

    if ( (n = socket(family, type, protocol)) < 0 ) {
        perr_exit("Socket error");
    }

    return n;
}
ssize_t Read(int fd, void *ptr, size_t nbytes)
{
    ssize_t n;

again:
    if ( (n = read(fd, ptr, nbytes)) == -1 ) {
        if (errno == EINTR) {
            goto again;
        }
    }

    return n;
}

ssize_t Write(int fd, const void *ptr, size_t nbytes)
{
    ssize_t n;

again:
    if ( (n = write(fd, ptr, nbytes)) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }

    return n;
}
int Close(int fd)
{
    int n;

    if ( (n = close(fd)) == -1 ) {
        perr_exit("Close error");
    }

    return n;
}

ssize_t Readn(int fd, void *vptr, size_t n)
{
    size_t   nleft;
    ssize_t  nright;
    char    *ptr;

    ptr = vptr;
    nleft = n;

    while (nleft > 0) {
        if ( (nright = read(fd, ptr, nleft)) < 0 ) {
             if (errno == EINTR) {
                nright = 0;
            } else {
                return nright; // return -1
            } // end if
        } else if ( nright == 0 ) {
            break; // break while
    } // end if
        nleft -= nright;
        ptr += nright;
    } // end while

    return n - nleft;
}

ssize_t Writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwriten;
    const char *ptr;

    ptr = vptr;
    nleft = n;

    while (nleft > 0) {
        if ( (nwriten = write(fd, ptr, nleft)) <= 0 ) {
            if ( nwriten < 0 && errno == EINTR) {
                nwriten = 0;
            } else {
                return -1;
            } // end if
        } // end if
        nleft -= nwriten;
        ptr += nwriten;
    } // end while

    return n;
}

static ssize_t my_read(int fd, char *ptr)
{
    static int read_cnt;
    static char *read_ptr;

    static char read_buf[100];

    if (read_cnt <= 0) {
again:
        if ( (read_cnt = read(fd, read_buf, sizeof(read_buf)))  < 0 ) {
            if (errno == EINTR) {
                goto again;
            } else {
                return -1;
            }
        } else if (read_cnt == 0) {
            return 0;
        } // end if
    } // end if
    read_cnt--;
    *ptr = *read_ptr++;
    return 1;
}

ssize_t Readline(int fd, void *vptr, size_t max_len)
{
    ssize_t n, rc;
    char    c, *ptr;

    ptr = vptr;

    for (n = 1; n < max_len; n++) {
        if ( (rc = my_read(fd, &c)) == 1 ) {
            *ptr++ = c;
            if (c == '\n') {
                break;
            }
        } else if (rc == 0) {
            *ptr = 0;
            return n - 1;
        } else {
            return -1;
        } // end if
    } // end for
    *ptr = 0;

    return n;
}