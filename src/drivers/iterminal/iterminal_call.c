int is_valid_action(VT_ACTION action) {

}
int call( device_t *dev , int token , char *out_buffer , int out_buffer_s ) {
    if ( ! PRIVATE(dev)->call_state ) { /* cmd line? */
        /* generate token */
        switch ( token ) {
            /*
             * some magic switch, that may be copied from previous shell
             */

            case TOKEN_ENTER:
                /* change call_state */
                /* create 2 pipe device
                    pipe_probe?
                */
                /* set stdio context */
                /* run job in new thread */
            break;

            default:
            /*
             * write to out_buffer as is
             */
            int ch = tocken && 0xFF
            if (ch == tocken) {
                /*
                 * echo
                 */
            } else {
                /*
                 * we shoud do smth
                 */
            }
            break;
        }
    } else {
        switch ( token ) {
            /*
             * some switch, that pick out only ^C like tokens
             *    ^C, ^S, ^Q
             */
            #if 0x0
            case TOKEN_SIGINT: /* ^C */
                /* change call_state */
                /* kill thread */
                /* destroy pipe devices */
            break;
            #endif
        }
    }
    return 0;
}


/* bla bla lba */
