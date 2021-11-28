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

#ifndef DORPC_NETWORKING_BUFFER_HPP
#define DORPC_NETWORKING_BUFFER_HPP

#include <msgpack.h>

#include <mpmc_pool.hpp>

struct Buffer {
	Buffer();
	~Buffer();
	
	msgpack_sbuffer buffer;
	Buffer* __m_next;
	
	static Buffer* Allocate();
	static void Fre(Buffer* buffer);
};

#endif

