/**
 * @file
 * @brief test page allocator
 * @details Some random or interactive tests for page allocator
 *
 * @date 09.05.10
 * @author Fedor Burdun
 */

#include <malloc.h>
#include <stdio.h>
#include <kernel/mm/mpallocator.h>

#include "config.h"

/**
 * pool
 */
char pool[PAGE_QUANTITY*CONFIG_PAGE_SIZE];
char *_heap_start = pool;
char *_heap_end = pool + PAGE_QUANTITY*CONFIG_PAGE_SIZE;

/**
 * print some information
 */
extern void multipage_info();

/**
 */
extern void multipage_init();

/**
 * set of page structure
 */
typedef struct test_list {
	void *page;
	struct test_list *next;
}test_list_t;

/**
 * alloc set of page
 */
test_list_t* page_set_alloc( int count ) {
	test_list_t* tmp = NULL;
	tmp = malloc(sizeof(test_list_t));
	tmp->next = NULL;
	tmp->page = mpalloc(count);
	#ifdef VERBOSE_DEBUG_OUT
	printf("ALLOC: %08x\n",tmp->page);
	multipage_info();
	#endif
	return tmp;
}

/**
 * free set of page
 */
void free_page_set(test_list_t *list) {
	mpfree(list->page);
	#ifdef VERBOSE_DEBUG_OUT
	printf("FREE: %08x\n",list->page);
	multipage_info();
	#endif
	free(list);
}

/**
 * stack as list
 */
typedef struct stack_ {
	test_list_t* list;
	struct stack_* next;
}stack_t;

stack_t *stack;

/**
 * push in stack
 */
void push(int count) {
	printf("\tpush %d pages: ",count);
	stack_t* tmp = malloc(sizeof(stack_t));
	#ifdef VERBOSE_DEBUG_OUT_MALLOC
	printf("\t\t\MALLOC (stack elem): %08x\n",tmp);
	#endif
	tmp->list = page_set_alloc(count);
	tmp->next = stack;
	stack = tmp;
	printf("\n");
}

/**
 * pop from stack
 */
void pop() {
	if (stack==NULL) {
		return;
	}
	printf("\tpop pages from ");
	stack_t* tmp;

	#ifdef STACK_POP_FROM_HEAD

	printf("head: ");
	tmp = stack->next;
	free_page_set(stack->list);
	#ifdef VERBOSE_DEBUG_OUT_MALLOC
	printf("\t\t\FREE (stack elem): %08x\n",stack);
	#endif
	free(stack);
	stack = tmp;

	#else /* if STACK POP FROM TAIL */

	printf("tail: ");

	if (stack->next == NULL) {
		tmp = stack->next;
		free_page_set(stack->list);
		#ifdef VERBOSE_DEBUG_OUT_MALLOC
		printf("\t\t\FREE (stack elem): %08x\n",stack);
		#endif
		free(stack);
		stack = tmp;
	} else {
		tmp = stack;
		while (tmp->next->next != NULL) {
			tmp = tmp->next;
		}
		free_page_set(tmp->next->list);
		#ifdef VERBOSE_DEBUG_OUT_MALLOC
		printf("\t\t\FREE (stack elem): %08x\n",stack);
		#endif
		free(tmp->next);
		tmp->next = NULL;
	}

	#endif
	printf("\n");
}

/**
 * Debug output memory (print list of markers) // must be tree in this allocator
 */
void do_allpage() {

}

/**
 * memory error counter
 */
int count_of_error = 0;

/**
 * simply memory checker (sum free and alocated memory must be equal size of pool)
 */
void memory_check() {
#if 0
	size_t allocp = allow_page_count();
	size_t freep  = free_page_count();
	printf("Allocated: %d ; Free %d \n", allocp , freep );
	if (allocp+freep != PAGE_QUANTITY) {
		printf("WARNING: Sum of allocated and free page don't equal page quality!!! \n");
		++count_of_error;
	}
#endif
}

int main() {
	printf("heap start: %16x ; heap end: %16x \n",(unsigned long) &_heap_start,(unsigned long) &_heap_end);

	multipage_init();
	multipage_info();

	printf("TEST PAGE_ALLOC NOW. COUNT OF PAGES: %ld\n",(long) PAGE_QUANTITY);
	const int max_page_for_alloc = MAX_PAGE_FOR_ALLOC;
	int test_count = TEST_COUNT;
	int tpcount,tptype;
	int i;

	#ifdef INTERACTIVE_TEST
	printf("Input: number of tests :: ");
	scanf("%d",&test_count);
	#endif
	for (i=0;i<test_count;++i) {
		#ifndef INTERACTIVE_TEST
		tpcount = random() % max_page_for_alloc + 1;
		tptype  = random() % 2;
		#else
		printf("Input: type action [ 0 - pop | 1 - push ] , number of page :: ");
		scanf("%d %d",&tptype,&tpcount);
		#endif

		printf("Test #%d: ",i);
		memory_check();

		if (stack!=NULL) {
			if (tptype) {
			#ifdef VERBOSE_DEBUG_OUT
				printf("PUSH:\n");
			#endif
				push(tpcount);
			} else {
			#ifdef VERBOSE_DEBUG_OUT
				printf("POP:\n");
			#endif
				pop();
			}
		} else {
			printf("PUSH because have NULL:\n");
			push(tpcount);
		}

		#ifdef VERBOSE_DEBUG_OUT
		do_allpage();
		#endif
	}

	while (stack!=NULL) {
		printf("(free last): ");
		memory_check();
		pop();
		#ifdef VERBOSE_DEBUG_OUT
		do_allpage();
		#endif
	}

	printf("END: ");
	memory_check();

	printf("\n\nMEMORY BAD SITUATION: %d\n",count_of_error);
}

