#include <stdio.h>
#include <string.h>

#define md5keyw "MD5_CHECKSUM"

void count()
{
   	FILE *pfile;
	pfile = fopen("imagetext.bin","rb");
	int k = fseek(pfile, 0, SEEK_END);
	if (k != 0) printf("Bad jump to the end of the file\n");
	long size = ftell(pfile);
	rewind(pfile);
	char *buffer =(char *) malloc(size);
	if (!buffer) printf("Bad memory allotment\n");
	size_t result = fread(buffer, 1, size, pfile);
	if (result != size){
	    printf("Bad copy to the buffer\n");
	}
	unsigned char *pmd5_sum = malloc(16*sizeof(unsigned char));
    pmd5_sum = (unsigned char *) count_md5((unsigned char *) buffer, (unsigned char *) buffer + size, (unsigned char *) pmd5_sum);
	int i;
    for (i = 0; i < 16; i++)
    	printf("%02x", pmd5_sum[i]);
    printf("\n");
    fclose(pfile);

	/*FILE *path;
	path = fopen("pathname.bin","r+b");
	if (!path) printf("An error during the openning file pathname.bin\n");
    char ch;
    int pathlength = 0;
    while ((ch = getchar( )) != '\n'){
        putc(ch, path);
        pathlength++;
	}
	pathlength++;
	int jumpbegin = fseek(path, 0, SEEK_SET);
	if (jumpbegin != 0) printf("Bad jump to the begin of the file");
	char *pathname =(char *) malloc(pathlength);
	pathname = fgets(pathname, pathlength, path);
	if (!pathname) printf("Bad memory allotment");
	fclose(path);

	FILE *pautoconf;
	pautoconf = fopen(pathname, "rb");
	if (!pautoconf) printf("Can't open the file autoconf.h\n");
    free(pathname);*/
    FILE *pautoconf;
    pautoconf = fopen("../autoconf.h", "rb");//pautoconf = fopen("//monitor/scripts/autoconf.h", "rb");
    if (!pautoconf) printf("Can't open the file autoconf.h\n");
    FILE *pcopyautoconf;
    pcopyautoconf = fopen("copyautoconf.h","wb");
    if (!pcopyautoconf) printf("Can't open the file copyautoconf.h\n");
    int find = 0;
    while (1){
    	char curch = fgetc(pautoconf);
    	int linelength = 1;
    	while (curch != '\n' && curch != EOF){
    		curch = fgetc(pautoconf);
    		linelength++;
    	}
    	if (curch == EOF){
    		break;
    	}
    	int jumpback = fseek(pautoconf, -linelength, SEEK_CUR);
    	if (jumpback != 0) printf("Bad jump to the beginning of the line\n");
    	char *line =(char *) malloc(linelength+1);
    	line = fgets(line, linelength+1, pautoconf);
    	fputs(line, pcopyautoconf);
    	if (!find) {
    		const char *findmd5keyw = strstr(line, md5keyw);
    		if (findmd5keyw){
    		    int tomd5 = linelength - (findmd5keyw - line + strlen(md5keyw) + 1);
    		    jumpback = fseek(pcopyautoconf, -tomd5, SEEK_CUR);
    		    if (jumpback != 0) printf("Bad jump to the md5 field of the line\n");
    			fputs("\"", pcopyautoconf);
    			int cur;
    			for (cur = 0; cur < 16; cur++){
    				int wrmd5i = fprintf(pcopyautoconf, "%02x", pmd5_sum[cur]);
    				if (wrmd5i < 0) printf("Incorrected written number of md5sum in copyautoconf.h\n");
                }
    			fputs("\"\n", pcopyautoconf);
    			find = 1;
    		}
        }
    	free(line);
    }
    fclose(pautoconf);
    fclose(pcopyautoconf);
    free(buffer);
    free(pmd5_sum);
}

main(){
    count();
}
