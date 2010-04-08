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
		printf("%d ",i);
		tmp = malloc(sizeof(test_list_t));
		tmp->page = page_alloc();                     /* CHECK THIS FUNCTION !!!!! */
		tmp->page = NULL;
		tmp->next = fr;
		fr = tmp;
	}
	return fr;
}

/**
 * free set of page
 */
void free_page_set(test_list_t *list) {
	test_list_t* cur = list;
	while (cur != NULL) {
		printf("* ");
		list = cur->next;
		//page_free(cur->page);                     /* CHECK THIS FUNCTION !!!!! */
		free(cur);
		cur = list;
	}
}

/**
 * count the number of free pages
 */
size_t free_page_count() {                     /* CHECK THIS FUNCTION !!!!! */
	/* go from prev to next */
	size_t fr = cmark_p->psize;
	pmark_t* cur = cmark_p->pnext;
	while (cur != cmark_p) {
		printf(". ");
		fr = fr + cur->psize;
		cur = cur->pnext;
	}
	return fr;
}

/**
 * count the number of allowed pages
 */
size_t allow_page_count() {                     /* CHECK THIS FUNCTION !!!!! */
	/* go from next to prev */
	size_t fr = cmark_p->psize;
	pmark_t* cur = cmark_p->pprev;
	while (cur != cmark_p) {
		printf(". ");
		fr = fr + cur->psize;
		cur = cur->pprev;
	}
	return PAGE_SIZE*PAGE_QUANTITY-fr;
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
	tmp->list = page_set_alloc(count);
	tmp->next = stack;
	stack = tmp;
	printf("\n");
}

/**
 * pop from stack
 */
void pop() {
	printf("\tpop pages: \n");
	stack_t* tmp = stack->next;
	free_page_set(stack->list);
	free(stack);
	stack = tmp;
}

int main() {
	printf("TEST PAGE_ALLOC NOW\n");
	const int test_count = 10;
	const int max_page_for_alloc = 20;
	int i;
	push(random() % max_page_for_alloc + 1);

	for (i=0;i<test_count;++i) {
		printf("Test #%d\n",i);
		printf("Test #%d: Allowed %ld Free %ld \n",i,allow_page_count(),free_page_count());
		if (stack!=NULL) {
			if (random()%2) {
				printf("push)\n");
				push(random() % max_page_for_alloc + 1);
			} else {
				printf("pop)\n");
				pop();
			}
		} else {
			push(random() % max_page_for_alloc +1 );
		}
	}

	while (stack!=NULL) {
		printf("Test #END\n",i);
		printf("Test #END (free last): Allowed %ld Free %ld \n",allow_page_count(),free_page_count());
		pop();
	}
}

