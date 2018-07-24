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

namespace Rtcr {
	struct Irq_session_info;
}

/**
 * State information about a IRQ session
 */
struct Rtcr::Irq_session_info : Session_rpc_info
{
	Irq_session_info(const char* creation_args, bool bootstrapped)
	:
		Session_rpc_info(creation_args, "", bootstrapped)
	{ }

	void print(Genode::Output &output) const
	{
		using Genode::Hex;

		Session_rpc_info::print(output);
	}
};

#endif /* _RTCR_IRQ_SESSION_INFO_H_ */
