/**
 * @file
 *
 * @date 1.12.10
 * @author Nikolay Korotky
 */
#include <types.h>
#include <hal/context.h>

void context_init(struct context *ctx, bool privileged) {
}

void context_set_stack(struct context *ctx, void *sp) {
}

void context_set_entry(struct context *ctx, void (*pc)(int), int arg) {
}

void context_switch(struct context *prev, struct context *next) {
}

