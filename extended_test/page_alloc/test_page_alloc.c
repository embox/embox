/**
 * @file
 * @brief test page allocator
 * @details
 *
 * @date 04.04.10
 * @author Fedor Burdun
 */

#include <malloc.h>
#include <stdio.h>
#include <lib/page_alloc.h>

#define NOVERBOSE_DEBUG_OUT

/**
 * set of page structure
 */
typedef struct test_list {
	pmark_t *page;
	struct test_list *next;
}test_list_t;

/**							//Have any troubles
 * alloc set of page
 */
test_list_t* page_set_alloc( int count ) {
	test_list_t* fr = NULL;
	test_list_t* tmp = NULL;
	int i;
	for (i=0;i<count;++i) {
		printf("%d ",i);
		tmp = malloc(sizeof(test_list_t));
		#ifdef VERBOSE_DEBUG_OUT
		printf("\t\t\tMALLOC (page set): %08x\n",tmp);
		#endif
		//tmp->page = page_alloc();                     /* CHECK THIS FUNCTION !!!!! */
		tmp->next = fr;
		fr = tmp;
	}
	return fr;
}

/**							//Have any troubles
 * free set of page
 */
void free_page_set(test_list_t *list) {
	test_list_t* cur = list;
	while (cur != NULL) {
		printf("* ");
		list = cur->next;
		//page_free(cur->page);                     /* CHECK THIS FUNCTION !!!!! */
		#ifdef VERBOSE_DEBUG_OUT
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
	/* go from prev to next */
	size_t fr = get_cmark_p()->psize;
	pmark_t* cur = get_cmark_p()->pnext;
	while (cur != get_cmark_p()) {
//		printf(". ");
		fr = fr + cur->psize;
		cur = cur->pnext;
	}
	return fr;
}

/**
 * count the number of allowed pages
 */
size_t allow_page_count() {
	/* go from next to prev */
	size_t fr = get_cmark_p()->psize;
	pmark_t* cur = get_cmark_p()->pprev;
	while (cur != get_cmark_p()) {
//		printf(". ");
		fr = fr + cur->psize;
		cur = cur->pprev;
	}
	//return PAGE_QUANTITY*PAGE_SIZE-fr;
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
	#ifdef VERBOSE_DEBUG_OUT
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
	printf("\tpop pages: ");
	stack_t* tmp = stack->next;
	free_page_set(stack->list);
	#ifdef VERBOSE_DEBUG_OUT
	printf("\t\t\FREE (stack elem): %08x\n",stack);
	#endif
	free(stack);
	stack = tmp;
	printf("\n");
}

/**
 * Debug output memory
 */
void do_allpage() {
	pmark_t* pcur = get_cmark_p();
	printf("Print map of memory: \n");
	do {
		printf("\tStruct: %08x\n\t\tsize: %d\n\t\tnext: %08x\n\t\tprev: %08x\n\n",
			pcur, pcur->psize, pcur->pnext, pcur->pprev);
		pcur = pcur->pnext;
	} while (pcur != get_cmark_p());
}

int main() {
	printf("TEST PAGE_ALLOC NOW. COUNT OF PAGES: %ld\n",(long) PAGE_QUANTITY);
	const int test_count = 1000;
	const int max_page_for_alloc = 2000;
	int i;

	page_alloc_init(); // WTF?! must be auto-init in function "page_alloc"
		//ok while test it, I don't run page_alloc =)

	#ifdef VERBOSE_DEBUG_OUT
	page_alloc_init();
	do_allpage();

	printf("Allowed %ld Free %ld \n",allow_page_count(), free_page_count());
	printf("PAGE ALLOC HAS INIT\n\n");
	#endif

#if 0

	void * page = page_alloc();

	push(1);
	do_allpage();

	push(2);
	do_allpage();

	push(4);
	do_allpage();

	page_free(page);
	do_allpage();

	pop();
	do_allpage();

	printf("Allowed %ld Free %ld \n",allow_page_count(), free_page_count());
#endif

//#if 0
	push(random() % max_page_for_alloc + 1);
	for (i=0;i<test_count;++i) {
		#ifdef VERBOSE_DEBUG_OUT
		printf("Test #%d\n",i);
		do_allpage();
		#endif
		printf("Test #%d: Allowed %ld Free %ld \n",i,allow_page_count(),
			free_page_count());

		if (stack!=NULL) {
			if (random()%2) {
			#ifdef VERBOSE_DEBUG_OUT
				printf("PUSH:\n");
			#endif
				push(random() % max_page_for_alloc + 1);
			} else {
			#ifdef VERBOSE_DEBUG_OUT
				printf("POP:\n");
			#endif
				pop();
			}
		} else {
			printf("PUSH because have NULL:\n");
			push(random() % max_page_for_alloc +1 );
		}
	}

	while (stack!=NULL) {
	#ifdef VERBOSE_DEBUG_OUT
		printf("Test #END\n",i);
	#endif
		printf("Test #END (free last): Allowed %ld Free %ld \n",allow_page_count(),
			free_page_count());
		pop();
	}
//#endif
}

