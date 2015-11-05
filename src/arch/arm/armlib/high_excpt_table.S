/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    17.08.2015
 */

.global setup_excpt_table
setup_excpt_table:
    ldr r0, =0xffff0000
    stmfd sp!, { lr }
    bl copy_excpt_table
    ldmfd sp!, { pc }
