/*
 *  This file is part of DORPC. Please see README for details.
 *  Copyright (C) 2022 Marek Zalewski aka Drwalin
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

#ifndef DORPC_RPC_FUNCTION_BASE_HPP
#define DORPC_RPC_FUNCTION_BASE_HPP

#include <unordered_map>
#include <functional>
#include <tuple>
#include <vector>
#include <cinttypes>
#include <atomic>

#include "../networking/Buffer.hpp"
#include "../serialization/serializator.hpp"

namespace rpc {
	class FunctionBase {
	public:
		
		virtual ~FunctionBase() = default;
		
		virtual void* GetPtr() = 0;
		
		virtual void Execute(serialization::Reader& reader) = 0;
		virtual void ExecuteWithReturn(serialization::Reader& reader,
				serialization::Writer& writerRet) = 0;
		
		inline uint32_t GetId() const { return id; }
		inline uint32_t SetId(uint32_t id) { return this->id = id; }
		
	protected:
		
		FunctionBase();
		
		uint32_t id;
		const uint32_t originalId;
	};
}

#endif

