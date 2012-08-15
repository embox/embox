/*
 * load_incbin.S
 *
 *  Created on: Aug 8, 2012
 *      Author: Eldar Abusalimov
 */

#include <framework/mod/options.h>

.section .load_image
.incbin OPTION_STRING_GET(image)

