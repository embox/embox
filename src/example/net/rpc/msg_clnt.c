/**
 * @file
 * @brief implementation of remote printing via RPC (client side)
 *
 * @date 28.05.12
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <net/lib/rpc/clnt.h>
#include <net/lib/rpc/xdr.h>
#include <framework/example/self.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

EMBOX_EXAMPLE(exec);

/**
 * RPC Server constants
 */
#define MESSAGEPROG 0x20000099
#define MESSAGEVERS 1
#define PRINTMESSAGE 1

static int exec(int argc, char *argv[])
{
	struct client *clnt;
	int clnt_res;
	char *server, *msg;
	struct timeval timeout = { 25, 0 };

	if (argc != 3) {
		printf("usage %s: <host> <message>\n", argv[0]);
		return -EINVAL;
	}

	server = argv[1];
	msg = argv[2];

	/* Create client handle for calling MESSAGEPROG on the server
	 * designated on the command line.  We tell the RPC package
	 * to use the udp protocol when contacting the server.
	 */
	clnt = clnt_create(server, MESSAGEPROG, MESSAGEVERS, "udp");
	if (clnt == NULL) {
		/*
		 * Couldn't establish connection with server.
		 * Print error message and die.
		 */
		clnt_pcreateerror(server);
		return -1;
	}

	/*
	 * Call the remote procedure "printmessage" on the server.
	 */
	if (clnt_call(clnt, PRINTMESSAGE,
		(xdrproc_t)xdr_wrapstring, (char *)&msg,
		(xdrproc_t)xdr_int, (char *)&clnt_res,
		timeout) != RPC_SUCCESS) {
		/*
		 * An error occurred while calling the server.
		 * Print error message and die
		 */
		clnt_perror(clnt, server);
		return -1;
	}

	clnt_destroy(clnt);

	/*
	 * Okay, we successfully called the remote procedure
	 */
	if (clnt_res == 0) {
		/*
		 * Server was unable to print our message.
		 * Print error message and die.
		 */
		printf("%s: %s couldn't print your message\n", argv[0], server);
		return -1;
	}

	/*
	 * Message got printed at server
	 */
	 printf("Message delivered to %s!\n", server);
	 return 0;
}
