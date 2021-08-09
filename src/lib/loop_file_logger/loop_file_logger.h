/**
 * @file
 *
 * @date Aug 3, 2021
 * @author Anton Bondarev
 */

#ifndef SRC_LIB_LOOP_FILE_LOGGER_LOOP_FILE_LOGGER_H_
#define SRC_LIB_LOOP_FILE_LOGGER_LOOP_FILE_LOGGER_H_


extern int loop_logger_write(char *message);
extern int loop_logger_read(int idx_mes, char *out_mes, int buf_size);
extern int loop_logger_size(void);
extern int loop_logger_message_size(void);


#endif /* SRC_LIB_LOOP_FILE_LOGGER_LOOP_FILE_LOGGER_H_ */
