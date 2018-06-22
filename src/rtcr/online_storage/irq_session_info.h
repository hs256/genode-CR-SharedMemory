/*
 * \brief  Stores Irq session state
 * \author Denis Huber
 * \date   2016-11-21
 */

#ifndef _RTCR_IRQ_SESSION_INFO_H_
#define _RTCR_IRQ_SESSION_INFO_H_

/* Genode includes */
#include <util/list.h>

/* Rtcr includes */
#include "../online_storage/info_structs.h"
#include "../online_storage/native_capability_info.h"
//#include "../online_storage/signal_context_info.h"
//#include "../online_storage/signal_source_info.h"

namespace Rtcr {
	struct Irq_session_info;
}

/**
 * State information about a IRQ session
 */
struct Rtcr::Irq_session_info : Session_rpc_info
{
	Genode::List<Native_capability_info> native_caps;
	//Genode::Capabilityy<Genode::Signal_source> const ss_cap;
	//Genode::List<Native_capability_info> native_caps;

	Irq_session_info(const char* creation_args, bool bootstrapped)
	:
		Session_rpc_info(creation_args, "", bootstrapped),
		native_caps()
	//	ss_cap  (ss_cap)
		//native_caps_lock(), native_caps()
	{ }

	/*Irq_session_info *find_by_badge(Genode::uint16_t badge)
	{
		if(badge == cap.local_name())
			return this;
		Irq_session_info *info = next();
		return info ? info->find_by_badge(badge) : 0;
	}*/
 
	void print(Genode::Output &output) const
	{
		using Genode::Hex;

		Session_rpc_info::print(output);
	}
};

#endif /* _RTCR_PD_SESSION_INFO_H_ */
