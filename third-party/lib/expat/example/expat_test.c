/*****************************************************************
 * outline.c
 *
 * Copyright 1999, Clark Cooper
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the license contained in the
 * COPYING file that comes with the expat distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Read an XML document from standard input and print an element
 * outline on standard output.
 * Must be used with Expat compiled for UTF-8 output.
 */

#include <stdio.h>
#include <lib/expat.h>

#if defined(__amigaos__) && defined(__USE_INLINE__)
#include <proto/expat.h>
#endif

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

#define BUFFSIZE        8192

char Buff[BUFFSIZE];

int Depth;

static void XMLCALL
startElement(void *userData, const char *name, const char **atts)
{
  int i;
  int *depthPtr = (int *)userData;
  for (i = 0; i < *depthPtr; i++)
    putchar('\t');
  puts(name);
  *depthPtr += 1;
}

static void XMLCALL
endElement(void *userData, const char *name)
{
  int *depthPtr = (int *)userData;
  *depthPtr -= 1;
}

void start_elements(void)
{
  char buf[BUFFSIZE];
  FILE *file;
  XML_Parser parser = XML_ParserCreate(NULL);
  int done;
  int depth = 0;
  file = fopen("about.html", "r");
  XML_SetUserData(parser, &depth);
  XML_SetElementHandler(parser, startElement, endElement);
  do {
    int len = (int)fread(buf, 1, sizeof(buf), file);
    done = len < sizeof(buf);
    if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
      fprintf(stderr,
              "%s at line %" XML_FMT_INT_MOD "u\n",
              XML_ErrorString(XML_GetErrorCode(parser)),
              XML_GetCurrentLineNumber(parser));
      return;
    }
  } while (!done);
  XML_ParserFree(parser);
  fclose(file);
}

static void XMLCALL
start(void *data, const char *el, const char **attr) {
	int i;

	for (i = 0; i < Depth; i++)
		printf("  ");

	printf("%s", el);

	for (i = 0; attr[i]; i += 2) {
		printf(" %s='%s'", attr[i], attr[i + 1]);
	}

	printf("\n");
	Depth++;
}

static void XMLCALL
end(void *data, const char *el) {
	Depth--;
}

void start_outline(void){
	XML_Parser p = XML_ParserCreate(NULL);
		FILE *file;
		if (!p) {
			fprintf(stderr, "Couldn't allocate memory for parser\n");
		}

		XML_SetElementHandler(p, start, end);

		file = fopen("src/about.html", "r");
		for (;;) {
			int done;
			int len;

			len = (int) fread(Buff, 1, BUFFSIZE, file);
			if (ferror(file)) {
				fprintf(stderr, "Read error\n");
			}
			done = feof(file);

			if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {
				fprintf(stderr, "Parse error at line %" XML_FMT_INT_MOD "u:\n%s\n",
						XML_GetCurrentLineNumber(p),
						XML_ErrorString(XML_GetErrorCode(p)));

			}
		}
		fclose(file);
}

#include <framework/example/self.h>

EMBOX_EXAMPLE(run);

static int run(int argc, char *argv[]) {
	//start_outline();
	start_elements();
	return 0;
}
