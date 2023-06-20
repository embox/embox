#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ism43362.h"
#include <drivers/gpio/gpio.h>

#define BUFFLEN 2048

static void get_command_string(char *b, int blen);
static void print_answer(char *b, int len);

int main() {
	char b[BUFFLEN]; b[BUFFLEN-1]=0;
	int len; // command length
	int c, tc;   // answer current & total counters
	
	printf("B-L475E-IOT01A WiFi testing\n");
	printf("Use x to exit,\n    <Tab> to send string,\n    <`> to insert <CR>,\n    <~> to insert <LF>\n");

	// Initialize WiFi module
	for (int res = ism43362_init(); res;) {printf("ISM43362 initialization error %d\n", res); return -1;}

	gpio_set(GPIO_PORT_C, 1<<9, GPIO_PIN_HIGH); // WiFi LED pin
	
	// AT command loop
	while (1) {
		get_command_string(b, BUFFLEN);

		if (b[0] == 'x') break;
		len = strlen(b);
		if (!len) continue;
		b[len] = '\r';
		len++;

		c = ism43362_exchange(b, len, b, BUFFLEN);
		tc = 0;

		// Remove leading CRLFs
		if (c >= 2 && b[0] == '\r' && b[1] == '\n') {c -= 2; memmove(b, b+2, c);}
		if (c >= 2 && b[0] == '\r' && b[1] == '\n') {c -= 2; memmove(b, b+2, c);}
		if (c < -1000000) {
			if (b[0] == '\r' && b[1] == '\n') {c += 2; memmove(b, b+2, -c-1000000);}
			if (b[0] == '\r' && b[1] == '\n') {c += 2; memmove(b, b+2, -c-1000000);}
		}

		// Cycle if buffer is not big enough
		while (c < -1000000) {
			c=-c - 1000000; 
			tc += c - 10; 
			print_answer(b, c - 10);
			memmove(b, b + c - 10, 10);
			c = ism43362_exchange(NULL, -1, b + 10, BUFFLEN-10);
			if (c >= 0) c += 10;
			if (c <= -1000000) c -= 10;
		}

		// Go to start of AT command loop if exchange error
		if (c<0) {printf("ism43362_exchange error #%d\n", c); continue;}
		
		// Remove answer tail (if OK)
		if (c >= 4 && b[c-4] == '\r' && b[c-3] == '\n' && b[c-2] == '>' && b[c-1] == ' ') c -= 4; // WiFi module prompt for new command
		if (c >= 2 && b[c-2] ==  'O' && b[c-1] ==  'K') c -= 2; // "OK" answer
		if (c >= 2 && b[c-2] == '\r' && b[c-1] == '\n') c -= 2; // Remove trailing CRLFs

		tc += c;

		//b[c] = 0; puts(b);
		print_answer(b, c);
		printf("\nTotal %d bytes in answer\n", tc);
	}
	
	gpio_setup_mode(GPIO_PORT_C, 1<<9, GPIO_MODE_IN);// WiFi LED pin
	return 0;
}

static void get_command_string(char *b, int bl) {
	printf("WiFi module>");
	fflush(stdout);

	for (int i = 0; i < bl; i++) {
		b[i] = getchar();
		if(b[i] == '\r' || i == bl-1) { 
			b[i] = 0;
			putchar('\r');
			if (b[0] == '\t') {
				sprintf(b, "S3=%04d", strlen(b+8));
				printf("WiFi module>%s`%s\n", b, b+8);
				b[7] = '\r';
				break;
			}
			putchar('\n');
			break;
		}
		if (i == 0 && b[i] == '\t') {
			i += 7;
			printf("\rWiFi module>S3=0000`");
			fflush(stdout);
			continue;
		}
		putchar(b[i]);
		if (b[i] == '`') b[i] = '\r';
		if (b[i] == '~') b[i] = '\n';
		fflush(stdout);
	}
}

static void print_answer(char *b, int c) {
	for (int i=0; i<c; i++) {
		if (0x20 <= b[i] && b[i] <= 0xdf) {putchar(b[i]); continue;}
		if (b[i] == '\r' || b[i] == '\n' || b[i] == '\t')  {putchar(b[i]); continue;}
		printf("\\x%02X", (int)b[i]);
	}
}


