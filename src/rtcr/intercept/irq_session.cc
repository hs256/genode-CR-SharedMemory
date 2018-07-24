/*
 * \brief  Intercepting Irq session
 * \author Harsha Sharma
 * \date   2018-07-04
 */

#include "irq_session.h"
#include <irq_session/irq_session.h>

using namespace Rtcr;


Irq_session_component::Irq_session_component(Genode::Env &env, Genode::Allocator &md_alloc, Genode::Entrypoint &ep,
		unsigned int irq, const char *creation_args, bool &bootstrap_phase)
:
	_env             (env),
	_md_alloc        (md_alloc),
	_ep              (ep),
	_bootstrap_phase (bootstrap_phase),
	_parent_irq 	 (env, irq),
	_parent_state    (creation_args, _bootstrap_phase)
{
	if(verbose_debug) Genode::log("\033[33m", "Irq", "\033[0m (parent ", _parent_irq, ")");

}

Irq_session_component::~Irq_session_component()
{
	if(verbose_debug) Genode::log("\033[33m", "~Irq", "\033[0m ", _parent_irq);
}

Irq_session_component *Irq_session_component::find_by_badge(Genode::uint16_t badge)
{
	if(badge == cap().local_name())
		return this;
	Irq_session_component *obj = next();
	return obj ? obj->find_by_badge(badge) : 0;
}

void Irq_session_component::ack_irq()
{
	_parent_irq.ack_irq();
}

void Irq_session_component::sigh(Genode::Signal_context_capability sigh)
{
	_parent_irq.sigh(sigh);
}

Genode::Irq_session::Info Irq_session_component::info()
{
	auto result = _parent_irq.info();
	return result;
}

Irq_session_component *Irq_root::_create_session(const char *args)
{
	if(verbose_debug) Genode::log("Irq_root::\033[33m", __func__, "\033[0m(", args,")");

	// Revert ram_quota calculation, because the monitor needs the original session creation argument
	char ram_quota_buf[32];
	char readjusted_args[160];
	unsigned int irq = 2;
	Genode::strncpy(readjusted_args, args, sizeof(readjusted_args));

	Genode::size_t readjusted_ram_quota = Genode::Arg_string::find_arg(readjusted_args, "ram_quota").ulong_value(0);
	readjusted_ram_quota = readjusted_ram_quota + sizeof(Irq_session_component) + md_alloc()->overhead(sizeof(Irq_session_component));

	Genode::snprintf(ram_quota_buf, sizeof(ram_quota_buf), "%zu", readjusted_ram_quota);
	Genode::Arg_string::set_arg(readjusted_args, sizeof(readjusted_args), "ram_quota", ram_quota_buf);

	// Create custom Irq_session
	Irq_session_component *new_session =
			new (md_alloc()) Irq_session_component(_env, _md_alloc, _ep, irq, readjusted_args, _bootstrap_phase);

	Genode::Lock::Guard lock(_objs_lock);
	_session_rpc_objs.insert(new_session);

	return new_session;
}


void Irq_root::_upgrade_session(Irq_session_component *session, const char *upgrade_args)
{
	if(verbose_debug) Genode::log("Irq_root::\033[33m", __func__, "\033[0m(session ", session->cap(),", args=", upgrade_args,")");

	char ram_quota_buf[32];
	char new_upgrade_args[160];

	Genode::strncpy(new_upgrade_args, session->parent_state().upgrade_args.string(), sizeof(new_upgrade_args));

	Genode::size_t ram_quota = Genode::Arg_string::find_arg(new_upgrade_args, "ram_quota").ulong_value(0);
	Genode::size_t extra_ram_quota = Genode::Arg_string::find_arg(upgrade_args, "ram_quota").ulong_value(0);
	ram_quota += extra_ram_quota;

	Genode::snprintf(ram_quota_buf, sizeof(ram_quota_buf), "%zu", ram_quota);
	Genode::Arg_string::set_arg(new_upgrade_args, sizeof(new_upgrade_args), "ram_quota", ram_quota_buf);

	session->parent_state().upgrade_args = new_upgrade_args;

	_env.parent().upgrade(session->parent_cap(), upgrade_args);
}


void Irq_root::_destroy_session(Irq_session_component *session)
{
	if(verbose_debug) Genode::log("Irq_root::\033[33m", __func__, "\033[0m(session ", session->cap(),")");

	_session_rpc_objs.remove(session);
	Genode::destroy(_md_alloc, session);
}


Irq_root::Irq_root(Genode::Env &env, Genode::Allocator &md_alloc, Genode::Entrypoint &session_ep,
		bool &bootstrap_phase)
:
	Root_component<Irq_session_component>(session_ep, md_alloc),
	_env              (env),
	_md_alloc         (md_alloc),
	_ep               (session_ep),
	_bootstrap_phase  (bootstrap_phase),
	_objs_lock        (),
	_session_rpc_objs ()
{
	if(verbose_debug) Genode::log("\033[33m", __func__, "\033[0m");
}


Irq_root::~Irq_root()
{
	while(Irq_session_component *obj = _session_rpc_objs.first())
	{
		_session_rpc_objs.remove(obj);
		Genode::destroy(_md_alloc, obj);
	}

	if(verbose_debug) Genode::log("\033[33m", __func__, "\033[0m");
}
