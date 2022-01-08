/**
 * @file
 *
 * @brief Testing interface for thread with different priority
 *
 * @date 2021.12.31
 * @author: kpishere
 */

#include <embox/test.h>

#include <util/dlist.h>
#include <util/priolist.h>
#include <util/log.h>
#include <framework/mod/options.h>

#define PRIORITY_INC 1

#define LIST_SIZE OPTION_GET(NUMBER, listsize)


int main(int argc, char **argv) {
	struct priolist testList;
	struct priolist_link testLinkItem[LIST_SIZE];

//	log_debug("Main process priority %d", priority);
	priolist_init(&testList);
	log_info("Init list id %X next %X prev %X poison %X",testList.node_list.list_id, testList.node_list.next, testList.node_list.prev, testList.node_list.poison);

	// Add
	for (int i = 0; i < LIST_SIZE; i++) {	
		// Init item to add to list
		priolist_link_init(&testLinkItem[i]);	

		// Set order of item in list
		testLinkItem[i].prio = i + PRIORITY_INC; 

		// Add item to list
		log_info("Add item prio %X noeId %X prioId %X",testLinkItem[i].prio,testLinkItem[i].node_link.list_id, testLinkItem[i].prio_link.list_id);
		priolist_add(&testLinkItem[i], &testList);
	}
	// Select and remove
	for (int i = 0; i < LIST_SIZE; i++) {
		struct priolist_link *nextItem = priolist_first(&testList);
		log_info("delete prio %X noeId %X prioId %X",nextItem->prio, nextItem->node_link.list_id, nextItem->prio_link.list_id);

		priolist_del(nextItem, &testList);
	}
    return 0;
}
