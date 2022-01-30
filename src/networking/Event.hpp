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

namespace net {
	class Event : public concurrent::node<Event> {
	public:

		void Run();
		static Event* Allocate();
		static void Free(Event* event);

		enum Type {
			CUSTOM,
			NONE,

			LISTEN_SOCKET_START,
			LISTEN_SOCKET_STOP,

			SOCKET_CONNECT,
			// SOCKET_RECONNECT,
			SOCKET_CLOSE,
			SOCKET_SEND,

			// LOOP_CLOSE,

			ALLCAST_LOOP,
			ALLCAST_CONTEXT,
			// MULTICAST
		};
		
		inline void MoveFrom(Event&& other) {
			after = std::move(other.after);
			data64 = std::move(other.data64);
			listenSocket = std::move(other.listenSocket);
			port = std::move(other.port);
			type = std::move(other.type);
			defer = std::move(other.defer);
		}

		std::function<void(Event&)> after;
		Buffer buffer_or_ip;
		union {
			struct Socket* socket;
			struct Context* context;
			struct Loop* loop;
			uint64_t data64;
			uint32_t data32;
			uint16_t data16;
			uint8_t data8;
		};
		struct us_listen_socket_t* listenSocket;
		int port;
		Type type;
		int defer;
	};
}

#endif

