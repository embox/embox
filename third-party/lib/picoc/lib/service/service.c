/* stdio.h library for large systems - small embedded systems use clibrary.c instead */
#ifndef BUILTIN_MINI_STDLIB

#include <errno.h>
#include <stdio.h>
#include "../interpreter.h"
#include "../picoc.h"



void StdioFclose(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = fclose(Param[0]->Val->Pointer);
}

void StdioFread(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = fread(Param[0]->Val->Pointer, Param[1]->Val->Integer, Param[2]->Val->Integer, Param[3]->Val->Pointer);
}

void StdioFwrite(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = fwrite(Param[0]->Val->Pointer, Param[1]->Val->Integer, Param[2]->Val->Integer, Param[3]->Val->Pointer);
}

void StdioFgetc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = fgetc(Param[0]->Val->Pointer);
}

void StdioFgets(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = fgets(Param[0]->Val->Pointer, Param[1]->Val->Integer, Param[2]->Val->Pointer);
}

void StdioRemove(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = remove(Param[0]->Val->Pointer);
}

void ServiceGetOutputFile(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
	ReturnValue->Val->Pointer = picoc_get_service_output_file();
}


/* handy structure definitions */
const char ServiceDefs[] = "\
typedef struct __ServiceStruct Service; \
";

/* all stdio functions */
struct LibraryFunction StdioFunctions[] =
{
    { StdioFopen,   "FILE *fopen(char *, char *);" },
    { StdioFreopen, "FILE *freopen(char *, char *, FILE *);" },
    { StdioFclose,  "int fclose(FILE *);" },
    { StdioFread,   "int fread(void *, int, int, FILE *);" },
    { StdioFwrite,  "int fwrite(void *, int, int, FILE *);" },
    { StdioFgetc,   "int fgetc(FILE *);" },
    { StdioFgetc,   "int getc(FILE *);" },
    { StdioFgets,   "char *fgets(char *, int, FILE *);" },
    { StdioFputc,   "int fputc(int, FILE *);" },
    { StdioFputs,   "int fputs(char *, FILE *);" },
    { StdioRemove,  "int remove(char *);" },
//    { StdioRename,  "int rename(char *, char *);" },
//    { StdioRewind,  "void rewind(FILE *);" },
//    { StdioTmpfile, "FILE *tmpfile();" },
//    { StdioClearerr,"void clearerr(FILE *);" },
    { StdioFeof,    "int feof(FILE *);" },
    { StdioFerror,  "int ferror(FILE *);" },
    { StdioFileno,  "int fileno(FILE *);" },
//    { StdioFflush,  "int fflush(FILE *);" },
//    { StdioFgetpos, "int fgetpos(FILE *, int *);" },
//    { StdioFsetpos, "int fsetpos(FILE *, int *);" },
//    { StdioFtell,   "int ftell(FILE *);" },
    { StdioFseek,   "int fseek(FILE *, int, int);" },
    { StdioPerror,  "void perror(char *);" },
    { StdioPutc,    "int putc(char *, FILE *);" },
    { StdioPutchar, "int putchar(int);" },
    { StdioPutchar, "int fputchar(int);" },
//    { StdioSetbuf,  "void setbuf(FILE *, char *);" },
//    { StdioSetvbuf, "void setvbuf(FILE *, char *, int, int);" },
    { StdioUngetc,  "int ungetc(int, FILE *);" },
    { StdioPuts,    "int puts(char *);" },
    { StdioGets,    "char *gets(char *);" },
    { StdioGetchar, "int getchar();" },
    { StdioPrintf,  "int printf(char *, ...);" },
    { StdioFprintf, "int fprintf(FILE *, char *, ...);" },
    { StdioSprintf, "int sprintf(char *, char *, ...);" },
    { StdioSnprintf,"int snprintf(char *, int, char *, ...);" },
    { StdioScanf,   "int scanf(char *, ...);" },
    { StdioFscanf,  "int fscanf(FILE *, char *, ...);" },
    { StdioSscanf,  "int sscanf(char *, char *, ...);" },
    { StdioVprintf, "int vprintf(char *, va_list);" },
    { StdioVfprintf,"int vfprintf(FILE *, char *, va_list);" },
    { StdioVsprintf,"int vsprintf(char *, char *, va_list);" },
    { StdioVsnprintf,"int vsnprintf(char *, int, char *, va_list);" },
    { StdioVscanf,   "int vscanf(char *, va_list);" },
    { StdioVfscanf,  "int vfscanf(FILE *, char *, va_list);" },
    { StdioVsscanf,  "int vsscanf(char *, char *, va_list);" },
    { NULL,         NULL }
};

/* creates various system-dependent definitions */
void StdioSetupFunc(Picoc *pc)
{
    struct ValueType *StructFileType;
    struct ValueType *FilePtrType;

    /* make a "struct __FILEStruct" which is the same size as a native FILE structure */
    StructFileType = TypeCreateOpaqueStruct(pc, NULL, TableStrRegister(pc, "__FILEStruct"), sizeof(FILE));

    /* get a FILE * type */
    FilePtrType = TypeGetMatching(pc, NULL, StructFileType, TypePointer, 0, pc->StrEmpty, TRUE);

    /* make a "struct __va_listStruct" which is the same size as our struct StdVararg */
    TypeCreateOpaqueStruct(pc, NULL, TableStrRegister(pc, "__va_listStruct"), sizeof(FILE));

    /* define EOF equal to the system EOF */
    VariableDefinePlatformVar(pc, NULL, "EOF", &pc->IntType, (union AnyValue *)&EOFValue, FALSE);
    VariableDefinePlatformVar(pc, NULL, "SEEK_SET", &pc->IntType, (union AnyValue *)&SEEK_SETValue, FALSE);
    VariableDefinePlatformVar(pc, NULL, "SEEK_CUR", &pc->IntType, (union AnyValue *)&SEEK_CURValue, FALSE);
    VariableDefinePlatformVar(pc, NULL, "SEEK_END", &pc->IntType, (union AnyValue *)&SEEK_ENDValue, FALSE);
    VariableDefinePlatformVar(pc, NULL, "BUFSIZ", &pc->IntType, (union AnyValue *)&BUFSIZValue, FALSE);
    VariableDefinePlatformVar(pc, NULL, "FILENAME_MAX", &pc->IntType, (union AnyValue *)&FILENAME_MAXValue, FALSE);
    VariableDefinePlatformVar(pc, NULL, "_IOFBF", &pc->IntType, (union AnyValue *)&_IOFBFValue, FALSE);
    VariableDefinePlatformVar(pc, NULL, "_IOLBF", &pc->IntType, (union AnyValue *)&_IOLBFValue, FALSE);
    VariableDefinePlatformVar(pc, NULL, "_IONBF", &pc->IntType, (union AnyValue *)&_IONBFValue, FALSE);
    VariableDefinePlatformVar(pc, NULL, "L_tmpnam", &pc->IntType, (union AnyValue *)&L_tmpnamValue, FALSE);
    VariableDefinePlatformVar(pc, NULL, "GETS_MAX", &pc->IntType, (union AnyValue *)&GETS_MAXValue, FALSE);

    /* define stdin, stdout and stderr */
    VariableDefinePlatformVar(pc, NULL, "stdin", FilePtrType, (union AnyValue *)&stdinValue, FALSE);
    VariableDefinePlatformVar(pc, NULL, "stdout", FilePtrType, (union AnyValue *)&stdoutValue, FALSE);
    VariableDefinePlatformVar(pc, NULL, "stderr", FilePtrType, (union AnyValue *)&stderrValue, FALSE);

    /* define NULL, TRUE and FALSE */
    if (!VariableDefined(pc, TableStrRegister(pc, "NULL")))
        VariableDefinePlatformVar(pc, NULL, "NULL", &pc->IntType, (union AnyValue *)&ZeroValue, FALSE);
}

/* portability-related I/O calls */
void PrintCh(char OutCh, FILE *Stream)
{
    putc(OutCh, Stream);
}

void PrintSimpleInt(long Num, FILE *Stream)
{
    fprintf(Stream, "%ld", Num);
}

void PrintStr(const char *Str, FILE *Stream)
{
    fputs(Str, Stream);
}

void PrintFP(double Num, FILE *Stream)
{
    fprintf(Stream, "%f", Num);
}

#endif /* !BUILTIN_MINI_STDLIB */
