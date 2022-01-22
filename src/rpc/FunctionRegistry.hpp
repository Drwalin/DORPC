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

#ifndef DORPC_RPC_FUNCTION_REGISTRY_HPP
#define DORPC_RPC_FUNCTION_REGISTRY_HPP

#include <unordered_map>
#include <functional>
#include <vector>
#include <cinttypes>

#include "FunctionBase.hpp"
#include "../serialization/serializator.hpp"

namespace rpc {
	class FunctionRegistry {
	public:
		
		static FunctionRegistry& Singleton();
		
		static void Add(FunctionBase* function);
		static void Remove(FunctionBase* function);
		static FunctionBase* GetById(uint32_t id);
		static FunctionBase* GetByPtr(void* ptr);
		
		static bool Call(serialization::Reader& args);
		static bool Call(serialization::Reader& args,
				serialization::Writer& returned);
		
		template<typename Func, Func func, typename... Args>
		static bool PrepareFunctionCall(serialization::Writer& writer,
				Args... args);
		
	private:
		
		FunctionRegistry() = default;
		
		std::unordered_map<uint32_t, FunctionBase*> functions;
		std::unordered_map<void*, FunctionBase*> functionsByPtr;
	};
}

#include "Function.hpp"

namespace rpc {
	template<typename Func, Func func, typename... Args>
	bool FunctionRegistry::PrepareFunctionCall(serialization::Writer& writer,
			Args... args) {
		FunctionBase* function = rpc::Function<Func, func>::Instance();
		if(function) {
			writer << function->GetId();
			writer << rpc::FunctionTraits<Func>::MakeTuple(args...);
			return true;
		}
		return false;
	}
}

#endif

