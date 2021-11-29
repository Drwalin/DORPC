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

#ifndef DORPC_UTIL_HPP
#define DORPC_UTIL_HPP

#include <msgpack.h>
#include "networking/Buffer.hpp"

#include <tuple>

namespace Util {
	namespace PackFor {
		template<typename... Args>
		inline void Foreach(Buffer* buffer, Args... args);
		
		template<typename T, typename... Args>
		inline void Foreach(Buffer* buffer, T object, Args... args) {
			buffer->PackAdd(object);
			Foreach(args...);
		}
		
		template<>
		inline void Foreach(Buffer* buffer) {}
	}
	
	template<class T, typename... Args>
	inline void Pack(Buffer* buffer, Args... args) {
		const int elements_count = sizeof...(args);
		PackFor::Foreach(buffer, args...);
	}

	
	
	template<class T, typename... Args>
	inline bool Unpack(std::tuple<Args...>& args, Buffer* buffer) {
		
		// TODO
		
		return false;
	}
	
	
	
	inline void MakeRPCHeader(Buffer* buffer) {
		// TODO
	}
}

#endif

