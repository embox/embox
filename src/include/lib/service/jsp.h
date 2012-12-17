#ifndef LIB_SERVICE_JSP_
#define LIB_SERVICE_JSP_

#include <lib/service/service.h>

extern int  service_jsp_interpret_init (int heap_size);
extern void service_jsp_interpret_fini (void);
extern void service_jsp_preprocess (FILE * in, FILE * out);
extern void service_jsp_interpret (struct service_file * file);

#endif
