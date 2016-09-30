
CFLAGS=-DUSE_IP_VER=4 -DUSE_CGI=0 -DUSE_REAL_CMD=0 -DUSE_PARALLEL_CGI=0

httpd : httpd.o httpd_cgi.o httpd_file.o \
	httpd_parselib.o httpd_parselib2.o \
	httpd_util.o
clean :
	-rm httpd *.o

