/** Function to shift the current view to the left/right
 *
 * @param: "arg->i" stores the number of tags to shift right (positive value)
 *          or left (negative value)
 */

void
shiftview(const Arg *arg) {
    Arg shifted;

    if(arg->i > 0) // left circular shift
        shifted.ui = (selmon->tagset[selmon->seltags] << arg->i)
           | (selmon->tagset[selmon->seltags] >> (LENGTH(tags) - arg->i));

    else // right circular shift
        shifted.ui = selmon->tagset[selmon->seltags] >> (- arg->i)
           | selmon->tagset[selmon->seltags] << (LENGTH(tags) + arg->i);

    view(&shifted);
}

void
shiftviewbusy(const Arg *arg) {
    Arg shifted;
    shifted.ui = selmon->tagset[selmon->seltags];
    Arg original;
    original.ui = shifted.ui;

    while (1) {
        if(arg->i > 0) // left circular shift
            shifted.ui = (shifted.ui << arg->i)
               | (shifted.ui >> (LENGTH(tags) - arg->i));

        else // right circular shift
            shifted.ui = (shifted.ui >> (- arg->i))
               | (shifted.ui << (LENGTH(tags) + arg->i));

        if (original.ui == shifted.ui) {
            // we have come full circle, give up
            return;
        }

        for (Client *c = selmon->clients; c != NULL; c = c->next) {
            if (c->tags & shifted.ui) {
                // there is a window in the new tagset
                view(&shifted);
                return;
            }
        }
    }
}
