/*
 *  This file is part of DORPC. Please see README for details.
 *  Copyright (C) 2021-2022 Marek Zalewski aka Drwalin
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

#ifndef DORPC_NETWORKING_EVENT_HPP
#define DORPC_NETWORKING_EVENT_HPP

#include <functional>

#include <concurrent.hpp>

#include "Buffer.hpp"

namespace networking {
	class Event : public concurrent::node<Event> {
	public:

		void Run();
		static Event* Allocate();
		static void Free(Event* event);

		enum Type {
			NONE,

			LISTEN_SOCKET_START,
			LISTEN_SOCKET_STOP,

			SOCKET_CONNECT,
			// SOCKET_RECONNECT,
			SOCKET_CLOSE,
			SOCKET_SEND,

			// LOOP_CLOSE,

			// ALLCAST,
			// MULTICAST
		};

		std::function<void(Event&)> after;
		Buffer buffer_or_ip;
		union {
			struct Socket* socket;
			struct Context* context;
			struct Loop* loop;
		};
		struct us_listen_socket_t* listenSocket;
		int port;
		Type type;
	};
}

#endif

