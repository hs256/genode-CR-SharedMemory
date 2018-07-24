/*
 * \brief  Structure for storing PD session information
 * \author Denis Huber
 * \date   2016-10-26
 */

#ifndef _RTCR_STORED_IRQ_SESSION_INFO_H_
#define _RTCR_STORED_IRQ_SESSION_INFO_H_

/* Genode includes */
#include <util/list.h>

/* Rtcr includes */
#include "../intercept/irq_session.h"
#include "../offline_storage/stored_info_structs.h"
#include "../offline_storage/stored_signal_context_info.h"
#include "../offline_storage/stored_signal_source_info.h"

namespace Rtcr {
	struct Stored_irq_session_info;
}


struct Rtcr::Stored_irq_session_info : Stored_session_info, Genode::List<Stored_irq_session_info>::Element
{
	Stored_irq_session_info(Irq_session_component &irq_session, Genode::addr_t targets_kcap)
	:
		Stored_session_info(irq_session.parent_state().creation_args.string(),
				irq_session.parent_state().upgrade_args.string(),
				targets_kcap,
				irq_session.cap().local_name(),
				irq_session.parent_state().bootstrapped)
	{ }

	Stored_irq_session_info *find_by_badge(Genode::uint16_t badge)
	{
		if(badge == this->badge)
			return this;
		Stored_irq_session_info *info = next();
		return info ? info->find_by_badge(badge) : 0;
	}

	void print(Genode::Output &output) const
	{
		using Genode::Hex;

		Stored_session_info::print(output);
	}

};

#endif /* _RTCR_STORED_IRQ_SESSION_INFO_H_ */
