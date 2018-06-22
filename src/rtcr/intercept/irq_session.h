/*
 * \brief  Intercepting irq session
 * \author Denis Huber
 * \date   2018-06-20
 */

#ifndef _RTCR_IRQ_SESSION_H_
#define _RTCR_IRQ_SESSION_H_

/* Genode includes */
#include <irq_session/connection.h>
#include <root/component.h>
#include <base/allocator.h>
#include <util/list.h>

/* Rtcr includes */
#include "../online_storage/irq_session_info.h"

namespace Rtcr {
	class Irq_session_component;
	class Irq_root;

	constexpr bool irq_verbose_debug = false;
	constexpr bool irq_root_verbose_debug = false;
}

/**
 * Custom RPC session object to intercept its creation, modification, and destruction through its interface
 */
class Rtcr::Irq_session_component : public Genode::Rpc_object<Genode::Irq_session>,
                                      public Genode::List<Irq_session_component>::Element
{
private:
	Genode::Env 		&_env;
	/**
	 * Enable log output for debugging
	 */
	static constexpr bool verbose_debug = irq_verbose_debug;
	/**
	 * Allocator for Rpc objects created by this session and also for monitoring structures
	 */
	Genode::Allocator  &_md_alloc;
	/**
	 * Entrypoint for managing created Rpc objects
	 */
	Genode::Entrypoint &_ep;
	/**
	* Reference to Target_child's bootstrap phase
	*/
	bool 		&_bootstrap_phase;
	/**
	 * Parent's session connection which is used by the intercepted methods
	 */
	Genode::Irq_connection 		_parent_irq;
	/**
	* State of parent's RPC object
	*/
	Irq_session_info 		_parent_state;	

public:
	Irq_session_component(Genode::Env &env, Genode::Allocator &md_alloc, Genode::Entrypoint &ep,
			const char *creation_args, bool &bootstrap_phase);
	~Irq_session_component();

	Genode::Irq_session_capability parent_cap() { return _parent_irq.cap(); }

	Irq_session_info &parent_state() { return _parent_state; }
	Irq_session_info const &parent_state() const { return _parent_state; }

	Irq_session_component *find_by_badge(Genode::uint16_t badge);

	/************************************
	 ** Signal session Rpc interface **
	 ************************************/

	void ack_irq() override;
	void sigh(Genode::Signal_context_capability sigh) override;
	Info info() override;	
};

/**
 * Custom root RPC object to intercept session RPC object creation, modification, and destruction through the root interface
 */
class Rtcr::Irq_root : public Genode::Root_component<Irq_session_component>
{
private:
	/**
	 * Enable log output for debugging
	 */
	static constexpr bool verbose_debug = irq_root_verbose_debug;

	/**
	 * Environment of Rtcr; is forwarded to a created session object
	 */
	Genode::Env        &_env;
	/**
	 * Allocator for session objects and monitoring list elements
	 */
	Genode::Allocator  &_md_alloc;
	/**
	 * Entrypoint for managing session objects
	 */
	Genode::Entrypoint &_ep;
	/**
	 * Reference to Target_child's bootstrap phase
	 */
	bool               &_bootstrap_phase;
	/**
	 * Lock for infos list
			 */
	Genode::Lock        _objs_lock;
	/**
	 * List for monitoring session objects
	 */
	Genode::List<Irq_session_component> _session_rpc_objs;

protected:
	Irq_session_component *_create_session(const char *args);
	void _upgrade_session(Irq_session_component *session, const char *upgrade_args);
	void _destroy_session(Irq_session_component *session);

public:
	Irq_root(Genode::Env &env, Genode::Allocator &md_alloc, Genode::Entrypoint &session_ep, bool &bootstrap_phase);
    ~Irq_root();
    
    Genode::List<Irq_session_component> &session_infos() { return _session_rpc_objs;  }
};

#endif /* _RTCR_IRQ_SESSION_H_ */
