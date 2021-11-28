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

#ifndef DORPC_NETWORKING_SOCKET_HPP
#define DORPC_NETWORKING_SOCKET_HPP

#include <cinttypes>

struct Socket {
	struct us_socket_t* socket;
	struct Context* context;
	struct Loop* loop;
	int ssl;
	
	
	void Init(struct us_socket_t* socket);
	void Destroy();
	
	
	void OnOpen(char* ip, int ipLength);
	void OnData(uint8_t* data, int length);
	void OnEnd();
	void OnClose(int code, void* reason);
	void OnTimeout();
	void OnWritable();
	
	static Socket* Make();
};

#endif

