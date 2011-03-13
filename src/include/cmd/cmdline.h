/**
 * @file
 * @brief Command line tokenizer.
 *
 * @date 13.03.2011
 * @author Eldar Abusalimo
 */

#ifndef CMD_CMDLINE_H_
#define CMD_CMDLINE_H_

// TODO in fact, this should be a part of shell interpreter, not cmd. -- Eldar
int cmdline_tokenize(char *cmdline, char **argv);

#endif /* CMD_CMDLINE_H_ */
