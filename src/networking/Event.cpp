/*
 *  This file is part of DORPC. Please see README for details.
 *  Copyright (C) 2021 Marek Zalewski aka Drwalin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
*
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#ifndef DORPC_NETWORKING_EVENT_CPP
#define DORPC_NETWORKING_EVENT_CPP

#include <libusockets.h>
#include <mpmc_pool.hpp>
#include <concurrent.hpp>

#include "Buffer.hpp"
#include "Socket.hpp"
#include "Loop.hpp"
#include "Context.hpp"

#include "Event.hpp"

namespace impl {
	concurrent::mpmc::pool<Event> eventPool;
}

void Event::Run() {
	switch(type) {
	case LISTEN_SOCKET_START:
		context->StartListening((const char*)buffer_or_ip.Data(), port);
		break;
	case LISTEN_SOCKET_STOP:
		context->listenSockets->erase(listenSocket);
		us_listen_socket_close(context->ssl, listenSocket);
		break;
		
	case SOCKET_CONNECT:
		context->InternalConnect((char*)buffer_or_ip.Data(), port);
		break;
	case SOCKET_CLOSE:
		socket->InternalClose();
		break;
	case SOCKET_SEND:
		socket->InternalSend(buffer_or_ip);
		break;
		
	default:
		break;
	}
	if(after)
		after(*this);
}

Event* Allocate() {
	return impl::eventPool.acquire();
}

void Free(Event* event) {
	if(event)
		impl::eventPool.release(event);
}

#endif

