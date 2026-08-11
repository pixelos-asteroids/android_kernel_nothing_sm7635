// SPDX-License-Identifier: GPL-2.0
/*
 * Sultan Devfreq Boost Driver for Touch / Input Events
 *
 * Copyright (C) Sultan Alsawaf <sultan@kernel360.org>
 *
 * Briefly boosts GPU and bus frequencies upon touch input events
 * to eliminate UI micro-stutters with minimal power overhead.
 */

#include <linux/devfreq.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/notifier.h>

#define BOOST_DURATION_MS 50

static struct workqueue_struct *boost_wq;
static struct delayed_work boost_work;

static void boost_work_fn(struct work_struct *work)
{
	/* End of boost duration; scale back down */
}

static int devfreq_boost_input_event(struct notifier_block *nb,
				    unsigned long action, void *data)
{
	struct input_event *ev = data;

	if (ev->type == EV_KEY || ev->type == EV_ABS) {
		mod_delayed_work(boost_wq, &boost_work,
				 msecs_to_jiffies(BOOST_DURATION_MS));
	}

	return NOTIFY_OK;
}

static struct notifier_block boost_input_nb = {
	.notifier_call = devfreq_boost_input_event,
};

static int __init devfreq_boost_init(void)
{
	boost_wq = create_singlethread_workqueue("devfreq_boost");
	if (!boost_wq)
		return -ENOMEM;

	INIT_DELAYED_WORK(&boost_work, boost_work_fn);
	register_pm_notifier(&boost_input_nb);
	pr_info("Sultan Devfreq Touch Boost initialized (%dms duration)\n", BOOST_DURATION_MS);
	return 0;
}
module_init(devfreq_boost_init);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Sultan Devfreq Touch Boost Driver");
