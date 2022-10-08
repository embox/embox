#include <cmsis_version.h>
#include <stdio.h>

void main(){
  printf("Cmsis version %d.%d", __CM_CMSIS_VERSION_MAIN,
         __CM_CMSIS_VERSION_SUB);
}