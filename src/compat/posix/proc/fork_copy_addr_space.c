/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */


extern void fork_addr_space_prepare_switch(void);
extern void fork_addr_space_finish_switch(void *safe_point);

void addr_space_prepare_switch(void) {
	fork_addr_space_prepare_switch();
}

void __addr_space_finish_switch(void *safe_point) {
	fork_addr_space_finish_switch(safe_point);
}
