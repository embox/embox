/**
 * @file
 * @brief TODO
 *
 * @date
 * @author
 */

void context_init(struct context *ctx, bool privileged) {
}

void context_set_stack(struct context *ctx, void *sp) {
}

void context_set_entry(struct context *ctx, void *pc) {
}

void context_switch(struct context *prev, struct context *next);
