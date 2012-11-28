#ifndef LIB_XWND_RESOURCES_H_
#define LIB_XWND_RESOURCES_H_

#define XAPP_REGISTER_RES_TYPE(type) typedef type __xwnd_resources_type;
#define XAPP_RESOURCES(xapp) ((__xwnd_resources_type *)(((struct xwnd_application*)xapp)->resources))

#endif
