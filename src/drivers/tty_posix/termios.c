#include <drivers/tty_posix/termios.h>

/*	tcsendbreak() - send a break			Author: Kees J. Bot
 *								13 Jan 1994
 */
int tcsendbreak(int fd, int duration) {
  return(tcioctl(fd, TCSBRK, &duration));
}
/*

Created:	July 26, 1994 by Philip Homburg
*/
int tcdrain(int fd) {
  return(tcioctl(fd, TCDRAIN, (void *)0));
}
/*	tcflush() - flush buffered characters		Author: Kees J. Bot
 *								13 Jan 1994
 */
int tcflush(int fd, int queue_selector) {
  return(tcioctl(fd, TCFLSH, &queue_selector));
}
/*
posix/_tcflow.c

Created:	June 8, 1993 by Philip Homburg
*/
int tcflow(int fd, int action) {
  return(tcioctl(fd, TCFLOW, &action));
}

int tcgetattr(fd, termios_p)
int fd;
struct termios *termios_p;
{
  return(ioctl(fd, TCGETS, termios_p));
}
/*
posix/_tcsetattr.c

Created:	June 11, 1993 by Philip Homburg
*/
int tcsetattr(fd, opt_actions, termios_p)
int fd;
int opt_actions;
_CONST struct termios *termios_p;
{
  int request;

  switch(opt_actions)
  {
    case TCSANOW:	request = TCSETS;	break;
    case TCSADRAIN:	request = TCSETSW;	break;
    case TCSAFLUSH:	request = TCSETSF;	break;
    default:		errno = EINVAL;		return(-1);
  };
  return(ioctl(fd, request, (void *) termios_p));
}

PUBLIC int ioctl(fd, request, data)
int fd;
int request;
void *data;
{
	//TODO
#if 0
	message m;

  m.TTY_LINE = fd;
  m.TTY_REQUEST = request;
  m.ADDRESS = (char *) data;
  return(_syscall(FS, IOCTL, &m));
#endif

}
