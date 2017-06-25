#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <embox/cmd.h>

EMBOX_CMD(hello_srs_main);

const int N = 10000;

void Multiply(int a[], int b[], int result[]) {

	for (int i = 1; i <= a[0]; i++) {
		for (int j = 1; j <= b[0]; j++) {
			result[i + j - 1] += a[i] * b[j];
		}
	}

	result[0] = a[0] + b[0] + 1;
}

void Assigment(int a[], int c[]) {
	for (int i = 1; i <= c[0]; i++){
		a[i] = c[i];
		c[i] = 0;
	}

	a[0] = c[0];
	c[0] = 0;

}

void Carry(int c[]) {
	int length = c[0];
	for (int i = 1; i <= c[0]; i++) {
		if (c[i] >= 10) {
			c[i + 1] += c[i] / 10;
			c[i] %= 10;
		}
	}

	while (c[length] == 0) {
		length--;
		c[0] = length;
	}
}

void Read(int numb, int a[]) {
	char* number = (char*)malloc(100);
	sprintf(number, "%d", numb);

	int index = 1;

	int i;
	for (i = strlen(number) - 1; i >= 0; i--) {
		a[index] = number[i] - '0';
		index++;
	}
	a[0] = strlen(number);
}


void Out1(int a[]) {
	for (int i = a[0]; i > 0; i--)
		printf("%d", a[i]);
	printf("\n");
}
void Out2(char* str) {
	for (int i = 0; i < strlen(str); i++)
		printf("%c", str[i]);
	printf("\n");
}

static int hello_srs_main(int argc, char *argv[] )
{
	int a[N], b[N], c[2 * N], flag = 0;
	char str[100];
	char* query=NULL;
	char* query1="fact";
	char* query2="open";
	const char *method = getenv("REQUEST_METHOD");
	assert(method);

 	if( !strcmp(method,"POST") )
	{
 		unsigned int len;
  		len = atoi( getenv("CONTENT_LENGTH") );
 		query = (char*)malloc(len+1);
  		fread(query, 1, len, stdin);
 		query[len] = 0;
	}
 	else if( !strcmp(method,"GET") )
 	{
 		query=(char*)malloc(strlen(getenv("QUERY_STRING"))+1);
 		strcpy(query,getenv("QUERY_STRING"));
	}
 	else printf("unknown REQUEST_METHOD\n");


	if ( !strncmp(query, query1, 4) )
	{
		flag = 1;
		Read(1, a);
		char* tmp = strstr(query, "=");
		query = NULL;
		for(int i = 1; i < sizeof(tmp); i++)
			query[i-1] = tmp[i];
		for (int i = 2; i <= atoi(query); i++) {
			Read(i, b);
			Multiply(a, b, c);
			Carry(c);
			Assigment(a, c);
		}
	}

	if ( !strncmp(query, query2, 4) )
	{
		flag = 2;
		char* tmp = strstr(query, "=");
		query = NULL;
		for(int i = 1; i < sizeof(tmp); i++)
			query[i-1] = tmp[i];
		FILE *File = fopen(query, "rt");
	if (File != NULL)
	{
		fgets(str, sizeof(str), File);
		fclose(File);
	}
	else
	{
		str[0] = 'E';
	}
	}

	printf(
		"HTTP/1.1 %d %s\r\n"
		"Content-Type: %s\r\n"
		"Connection: close\r\n"
		"\r\n", 200, "OK", "text/html");

	printf("<HTML>"
		"<HEAD>"
   		"<TITLE>Fact</TITLE>"
		"</HEAD>"
		"<BODY>"
		"Body: ");
	if (flag == 1)
		Out1(a);
	else
	if (flag == 2)
		Out2(str);

	printf(
		"</BODY>"
		"</HTML>");

	free(query);
	fflush(stdout);
	return 0;
}

