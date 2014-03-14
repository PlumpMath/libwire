#include "wire_wait.h"
#include "wire.h"

void wire_wait_list_init(wire_wait_list_t *wl)
{
	list_head_init(&wl->head);
}

void wire_wait_init(wire_wait_t *w)
{
	list_head_init(&w->list);
	w->start = NULL;
	w->waiting = 0;
	w->triggered = 0;
}

void wire_wait_chain(wire_wait_list_t *wl, wire_wait_t *w)
{
	list_add_tail(&w->list, &wl->head);
	w->start = wl;
	w->waiting = 1;
	w->triggered = 0;
}

void wire_wait_unchain(wire_wait_t *w)
{
	list_del(&w->list);
	w->start = NULL;
}

wire_wait_t *wire_list_wait(wire_wait_list_t *wl)
{
	struct list_head *cur;

	wl->wire = wire_get_current();

	while (1) {
		wire_suspend();

		for (cur = wl->head.next; cur != &wl->head; cur = cur->next) {
			wire_wait_t *cur_w = list_entry(cur, wire_wait_t, list);
			if (cur_w->triggered)
				return cur_w;
		}
	}
}

void wire_wait_resume(wire_wait_t *w)
{
	if (w->waiting) {
		w->triggered = 1;
		wire_resume(w->start->wire);
	}
}

void wire_wait_reset(wire_wait_t *w)
{
	w->waiting = 1;
	w->triggered = 0;
}

void wire_wait_stop(wire_wait_t *w)
{
	w->waiting = 0;
	w->triggered = 0;
}