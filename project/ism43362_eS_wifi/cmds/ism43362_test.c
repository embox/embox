#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ism43362.h"
#include <drivers/gpio.h>

#define BUFFER_SIZE 2048
#define WIFI_LED_PIN (1 << 9)

static void get_command_string(char *buffer, int buffer_length) {
	printf("WiFi module>");
	fflush(stdout);

	for (int i = 0; i < buffer_length; i++) {
		buffer[i] = getchar();

		if (buffer[i] == '\r' || i == buffer_length - 1) { 
			buffer[i] = '\0';
			putchar('\r');

			if (buffer[0] == '\t') { 
				sprintf(buffer, "S3=%04d", strlen(buffer + 8));
				printf("WiFi module>%s`%s\n", buffer, buffer + 8);
				buffer[7] = '\r';
				break;
			}
			putchar('\n');
			break;
		}

		if (i == 0 && buffer[i] == '\t') {
			i += 7;
			printf("\rWiFi module>S3=0000`");
			fflush(stdout);
			continue;
		}

		putchar(buffer[i]);
		if (buffer[i] == '`') buffer[i] = '\r';
		if (buffer[i] == '~') buffer[i] = '\n';

		fflush(stdout);
	}
}

static void print_response(char *buffer, int c) {
	for (int i=0; i<c; i++) {
		if (0x20 <= buffer[i] && buffer[i] <= 0xdf) {
			putchar(buffer[i]);
		} else if (buffer[i] == '\r' || buffer[i] == '\n' || buffer[i] == '\t'){
			putchar(buffer[i]);
		} else {
			printf("\\x%02X", (int)buffer[i]);
		}
	}
}
void print_wifi_setup_guide() {
    printf("\n=== B-L475E-IOT01A Wi-Fi Configuration Guide ===\n\n");
    
    printf("BASIC USAGE:\n");
    printf("1. Enter AT commands directly\n");
    printf("2. End each command with Enter (or ` symbol)\n");
    printf("3. Special keys:\n");
    printf("   - 'x' = Exit program\n");
    printf("   - Tab = Send string with length prefix\n");
    printf("   - `   = Carriage Return (CR)\n");
    printf("   - ~   = Line Feed (LF)\n\n");
    
    printf("STEP-BY-STEP SETUP:\n");
    printf("1. Configure SSID:\n");
    printf("   C1=Your_WiFi_Name`\n\n");
    
    printf("2. Set password:\n");
    printf("   C2=Your_WiFi_Password`\n\n");
    
    printf("3. Select security (4 for WPA2):\n");
    printf("   C3=4`\n\n");
    
    printf("4. Enable DHCP (1 for ON):\n");
    printf("   C4=1`\n\n");
    
    printf("5. Connect to network:\n");
    printf("   C0`\n\n");
    
    printf("6. Verify connection (should return '1'):\n");
    printf("   CS`\n\n");
    
    printf("TROUBLESHOOTING:\n");
    printf("- If connection fails, check:\n");
    printf("  1. Correct SSID/password\n");
    printf("  2. Matching security type\n");
    printf("  3. DHCP settings\n");
    
    printf("EXAMPLE COMPLETE SETUP:\n");
    printf("C1=MyHomeWiFi`\n");
    printf("C2=SecurePassword123`\n");
    printf("C3=4`\n");
    printf("C4=1`\n");
    printf("C0`\n");
    printf("CS`\n");
}

static void clear_leading_crlf(char *buffer, int *length) {
    while (*length >= 2 && buffer[0] == '\r' && buffer[1] == '\n') {
        *length -= 2;
        memmove(buffer, buffer + 2, *length);
    }
}

int main() {
	char buffer[BUFFER_SIZE];
	buffer[BUFFER_SIZE - 1]=0;
	int command_len; // command length
	int c;
	int total_c;   // answer current & total counters
	
	print_wifi_setup_guide();

	// Initialize WiFi module
	int result = ism43362_init();
    if (result != 0) {
        printf("ISM43362 initialization error %d\n", result);
        return -1;
    }
	gpio_set(GPIO_PORT_C, WIFI_LED_PIN, GPIO_PIN_HIGH); // WiFi LED pin
		

	// AT command loop
	while (1) {
		get_command_string(buffer, BUFFER_SIZE);

		if (buffer[0] == 'x'){
			break;
		}
		command_len = strlen(buffer);

		if (!command_len){
			continue;
		}

		buffer[command_len] = '\r';
		command_len++;

		c = ism43362_exchange(buffer, command_len, buffer, BUFFER_SIZE);
		if (c < -10000) {
			printf("ism43362_exchange error #%d\n", c); 
			continue;
		} // Go to start of AT command loop (exchange error)
		
		total_c = 0;

		// Remove leading CRLFs
		clear_leading_crlf(buffer, &c);

		// Cycle if buffer is not big enough
		while (c < 0) {
			c = -c; 
			total_c += c - 10; 
			print_response(buffer, c - 10);
			memmove(buffer, buffer + c - 10, 10);
			c = ism43362_exchange(NULL, -1, buffer + 10, BUFFER_SIZE - 10);

			if (c < -10000) {
				printf("ism43362_exchange error #%d\n", c);
				continue;
			} // Go to start of AT command loop (exchange error)
			
			if (c < 0){
				c -= 10;
			} else {
				c += 10;
			}
		}

		// Remove answer tail (if OK)
		if (c >= 4 && buffer[c - 4] == '\r' && buffer[c - 3] == '\n' && buffer[c - 2] == '>' && buffer[c - 1] == ' ')
			c -= 4; // WiFi module prompt for new command
		if (c >= 2 && buffer[c - 2] ==  'O' && buffer[c - 1] ==  'K')
			c -= 2; // "OK" answer
		if (c >= 2 && buffer[c - 2] == '\r' && buffer[c - 1] == '\n')
			c -= 2; // Remove trailing CRLFs

		total_c += c;

		//b[c] = 0; puts(b);
		print_response(buffer, c);
		printf("\nTotal %d bytes in answer\n", total_c);
	}
	
	gpio_setup_mode(GPIO_PORT_C, WIFI_LED_PIN, GPIO_MODE_IN);// WiFi LED pin
	return 0;
}


