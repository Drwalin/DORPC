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

#ifndef DORPC_NETWORKING_CONTEXT_HPP
#define DORPC_NETWORKING_CONTEXT_HPP

struct Context {
	struct us_context_t* context;
	struct Loop* loop;
	int ssl;
	
	
	void Init(struct us_context_t* context, int ssl);
	void Destroy();
	
	
	static void InternalOnOpenTcp(struct us_socket_t* socket, int isClient,
			char* ip, int ipLength);
	static void InternalOnOpenSsl(struct us_socket_t* socket, int isClient,
			char* ip, int ipLength);
	static void InternalOnData(struct us_socket_t* socket, char* data,
			int length);
	static void InternalOnEnd(struct us_socket_t* socket);
	static void InternalOnClose(struct us_socket_t* socket, int code,
			void* reason);
	static void InternalOnTimeout(struct us_socket_t* socket);
	static void InternalOnWritable(struct us_socket_t* socket);
	
	static Context* Make();
};

#endif

