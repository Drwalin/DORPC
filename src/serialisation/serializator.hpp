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

#ifndef DORPC_SERIALIZATION_SERIALIZATOR_HPP
#define DORPC_SERIALIZATION_SERIALIZATOR_HPP

#include <vector>
#include <string>
#include <string_view>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

namespace serialisation {
	class writer {
	public:
		
	public:
		
	};
	
	
	
	class reader {
	public:		
		
		uint32_t read_array_header();
		uint32_t read_map_header();
		uint32_t read_string_header();
		
		uint64_t read_int();
		
	public:
		std::istream& stream;
	};
	
	inline reader& operator >> (reader& r, uint64_t& v) {
	}
}

#endif

