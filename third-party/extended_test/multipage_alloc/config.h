
#define HAND_TEST_1
//#define COMP_TEST_1

#ifdef HAND_TEST_1
//#define DONOTUSE_PAGE_ALLOC 		/* for testing test of page alloc =) */
#define INTERACTIVE_TEST			/* */
#define VERBOSE_DEBUG_OUT			/* Some advanced output (list of page marks) */
//#define VERBOSE_DEBUG_OUT_MALLOC
#define STACK_POP_FROM_HEAD		/* or tail */ // I'm not sure that is working
#define TEST_COUNT 2
#define MAX_PAGE_FOR_ALLOC 10
#define CONFIG_PAGE_SIZE 0x1000
#define PAGE_QUANTITY 16
#endif

#ifdef HAND_TEST_0
//#define DONOTUSE_PAGE_ALLOC 		/* for testing test of page alloc =) */
#define INTERACTIVE_TEST			/* */
#define VERBOSE_DEBUG_OUT			/* Some advanced output (list of page marks) */
//#define VERBOSE_DEBUG_OUT_MALLOC
#define STACK_POP_FROM_HEAD		/* or tail */ // I'm not sure that is working
#define TEST_COUNT 2
#define MAX_PAGE_FOR_ALLOC 10
#define CONFIG_PAGE_SIZE 8
#define PAGE_QUANTITY 8
#endif

#ifdef COMP_TEST_1
//#define DONOTUSE_PAGE_ALLOC 		/* for testing test of page alloc =) */
//#define INTERACTIVE_TEST			/* */
//#define VERBOSE_DEBUG_OUT			/* Some advanced output (list of page marks) */
//#define VERBOSE_DEBUG_OUT_MALLOC
//#define STACK_POP_FROM_HEAD		/* or tail */ // I'm not sure that is working
#define TEST_COUNT 200000
#define MAX_PAGE_FOR_ALLOC 0x0f00
#define CONFIG_PAGE_SIZE 0x2000
#define PAGE_QUANTITY 0x1000
#endif
