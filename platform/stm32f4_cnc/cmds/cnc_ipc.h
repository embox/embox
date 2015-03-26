
#ifndef _CNC_IPC_H
#define _CNC_IPC_H

#define CNCIPC_VAR "CNCIPC"
#define CNCIPC_IN CNCIPC_VAR "I"
#define CNCIPC_OUT CNCIPC_VAR "O"
#define CNCIPC_IO CNCIPC_VAR "X"
#define CNCIPC_ENV CNCIPC_IO "=DDD"
#define CNCIPC_ENV_LEN (strlen(CNCIPC_ENV))

#endif /* _CNC_IPC_H */
