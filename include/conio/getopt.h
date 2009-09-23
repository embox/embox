/**
 * \file getopt.h
 * \date 23.09.09
 * \author Sikmir
 */
#ifndef _GETOPT_H
/*
struct option {
    const char *name;
    int has_arg;
    int *flag;
    int val;
};

int getopt_long(int argc, char **argv, const char *shortopts, const struct option *longopts, int *longind);
*/

#define MAX_SHELL_KEYS 12

typedef struct {
        char name;
        char *value;
} SHELL_KEY;

/**
 * parse arguments array on keys-value structures and entered amount of entered keys otherwise (if everything's OK)
 * @return  -1 wrong condition found. Arguments not in format: -<key> [<value>]
 * @return  -2 too many keys entered
 * @return -3 wrong key
 * @return 0 if (if everything's OK)
 */
int parse_arg(const char *handler_name, int argsc, char **argsv,
              char *available_keys, int amount_of_available_keys, SHELL_KEY *keys);

/**
 * Compare keys with available
 * @returns TRUE if all keys presented are available, FALSE otherwise
 */
/*int check_keys(SHELL_KEY *keys, int amount, char *available_keys,
        int amount_of_available_keys);*/

/**
 * Determines whether key was entered
 * @returns TRUE if key is active (was entered by user) and fills value
 */
int get_key(char key, SHELL_KEY *keys, int keys_amount, char **value);

#endif /* _GETOPT_H */
