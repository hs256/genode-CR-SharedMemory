/*
 * \brief Child creation
 * \author Denis Huber
 * \date 2016-08-04
 */

#ifndef _RTCR_TARGET_CHILD_H_
#define _RTCR_TARGET_CHILD_H_

/* Genode includes */
#include <base/child.h>
#include <base/env.h>
/*#include <rom_session/connection.h>
#include <ram_session/connection.h>
#include <cpu_session/connection.h>
#include <pd_session/connection.h>
#include <region_map/client.h>*/

/* Rtcr includes */
#include "pd_session_component.h"
#include "cpu_session_component.h"
#include "ram_session_component.h"

namespace Rtcr {
	class Target_child;
	using namespace Genode;
}

class Rtcr::Target_child
{
private:
	static constexpr bool verbose = true;
	/**
	 * Checkpointer's environment
	 */
	Env        &_env;
	Allocator  &_md_alloc;
	/**
	 * Child's Entrypoint
	 */
	const char *_unique_name;

	struct Pd_manager
	{
		const char *label;
		Rtcr::Pd_session_component pd;

		Pd_manager(Env &env, Allocator &md_alloc, const char *unique_name)
		:
			label(unique_name),
			pd(env, md_alloc, label)
		{
			log("Pd session managed");
		}
	} _pd_manager;

	struct Cpu_manager
	{
		Rtcr::Cpu_session_component cpu;

		Cpu_manager(Env &env, Allocator &md_alloc, Pd_session_capability parent_pd_cap)
		:
			cpu(env, md_alloc, parent_pd_cap)
		{
			log("Cpu session managed");
		}
	} _cpu_manager;

	struct Ram_manager
	{
		const size_t donate_quota = 1*1024*1024;
		Rtcr::Ram_session_component ram;

		Ram_manager(Env &env, Allocator &md_alloc)
		:
			ram(env, md_alloc)
		{
			log("ref_account: ", ram.ref_account(env.ram_session_cap()));
			log("transfer result: ", env.ram().transfer_quota(ram.cap(), donate_quota));
			log("Ram quota ", ram.quota());
			log("Ram session managed");
		}
	} _ram_manager;

public:

	/**
	 * Constructor
	 */
	Target_child(Env &env, Allocator &md_alloc, const char *unique_name)
	:
		_env(env), _md_alloc(md_alloc),
		_unique_name(unique_name),
		_pd_manager(_env, _md_alloc, _unique_name),
		_cpu_manager(_env, _md_alloc, _pd_manager.pd.parent_pd_cap()),
		_ram_manager(_env, _md_alloc)
	{
		if(verbose) log("Target_child created");
	}

};

#endif /* _RTCR_TARGET_CHILD_H_ */