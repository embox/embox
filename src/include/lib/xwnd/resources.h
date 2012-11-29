#ifndef LIB_XWND_RESOURCES_H_
#define LIB_XWND_RESOURCES_H_

#define XAPP_REGISTER_RES_TYPE(type) typedef typeof(type) __xwnd_resources_type;
#define XAPP_RESOURCES(xapp) ((__xwnd_resources_type *)((xapp)->resources))
#define XAPP_INIT_RESOURCES(xapp,res_inst) ( ((xapp)->resources) = &(res_inst) )

#endif
