/**
 * @file
 * @brief test page allocator
 * @details Some random or interactive tests for page allocator
 *
 * @date 04.04.10
 * @author Fedor Burdun
 */

#include <malloc.h>
#include <stdio.h>
#include <kernal/mm/opallcator.h>

#include "config.h"

typedef struct pmark {
	size_t psize;
	struct pmark *pnext;
	struct pmark *pprev;
}pmark_t;

/**
 * set of page structure
 */
typedef struct test_list {
	pmark_t *page;
	struct test_list *next;
}test_list_t;

/**
 * alloc set of page
 */
test_list_t* page_set_alloc( int count ) {
	test_list_t* fr = NULL;
	test_list_t* tmp = NULL;
	int i;
	for (i=0;i<count;++i) {
		#ifdef VERBOSE_DEBUG_OUT
		printf("%d ",i);
		#endif
		tmp = malloc(sizeof(test_list_t));
		#ifdef VERBOSE_DEBUG_OUT_MALLOC
		printf("\t\t\tMALLOC (page set): %08x\n",tmp);
		#endif
		#ifndef DONOTUSE_PAGE_ALLOC
		if (!(tmp->page = page_alloc())) {
			#ifdef VERBOSE_DEBUG_OUT
			printf("\t\tPAGE_ALLOC: No memory enough\n");
			#else
			printf("%d ", i);
			#endif
			free(tmp);
			return fr;
		}
		tmp->next = fr;
		fr = tmp;
		#endif
	}
	return fr;
}

/**
 * free set of page
 */
void free_page_set(test_list_t *list) {
	test_list_t* cur = list;
	while (cur != NULL) {
		#ifdef VERBOSE_DEBUG_OUT
		printf("* ");
		#endif
		list = cur->next;
		#ifndef DONOTUSE_PAGE_ALLOC
		page_free(cur->page);
		#endif
		#ifdef VERBOSE_DEBUG_OUT_MALLOC
		printf("\t\t\tFREE (page set): %08x\n",cur);
		#endif
		free(cur);
		cur = list;
	}
}

/**
 * count the number of free pages
 */
size_t free_page_count() {
	#ifdef DONOTUSE_PAGE_ALLOC
	return 0;
	#endif
	/* go from prev to next */
	if (get_cmark_p()==NULL) {
		return 0;
	}
	size_t fr = get_cmark_p()->psize;
	pmark_t* cur = get_cmark_p()->pnext;
	while (cur != get_cmark_p()) {
		fr = fr + cur->psize;
		cur = cur->pnext;
	}
	return fr;
}

/**
 * count the number of allowed pages
 */
size_t allow_page_count() {
	#ifdef DONOTUSE_PAGE_ALLOC
	return 0;
	#endif
	/* go from next to prev */
	if (get_cmark_p()==NULL) {
		return PAGE_QUANTITY;
	}
	size_t fr = get_cmark_p()->psize;
	pmark_t* cur = get_cmark_p()->pprev;
	while (cur != get_cmark_p()) {
		fr = fr + cur->psize;
		cur = cur->pprev;
	}
	return PAGE_QUANTITY-fr;
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
 * Debug output memory (print list of markers)
 */
void do_allpage() {
	pmark_t* pcur = get_cmark_p();
	printf("Print map of memory: \n");
	if (pcur == NULL) { /* don't exist list of empty page */
		printf("\tList of free page is empty\n");
		return;
	}

	do {
		printf("\tStruct: %08x\n\t\tsize: %d\n\t\tnext: %08x\n\t\tprev: %08x\n\n",
			pcur, pcur->psize, pcur->pnext, pcur->pprev);
		pcur = pcur->pnext;
	} while (pcur != get_cmark_p());
}

/**
 * memory error counter
 */
int count_of_error = 0;

/**
 * simply memory checker (sum free and alocated memory must be equal size of pool)
 */
void memory_check() {
	size_t allocp = allow_page_count();
	size_t freep  = free_page_count();
	printf("Allocated: %d ; Free %d \n", allocp , freep );
	if (allocp+freep != PAGE_QUANTITY) {
		printf("WARNING: Sum of allocated and free page don't equal page quality!!! \n");
		++count_of_error;
	}
}

int main() {
	printf("TEST PAGE_ALLOC NOW. COUNT OF PAGES: %ld\n",(long) PAGE_QUANTITY);
	const int max_page_for_alloc = MAX_PAGE_FOR_ALLOC;
	int test_count = TEST_COUNT;
	int tpcount,tptype;
	int i;

	page_alloc_init();

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

