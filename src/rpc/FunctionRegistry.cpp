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

#include "FunctionRegistry.hpp"

namespace rpc {
	
	FunctionRegistry& FunctionRegistry::Singleton() {
		static FunctionRegistry singleton;
		return singleton;
	}
	
	void FunctionRegistry::Remove(FunctionBase* function) {
		Singleton().functions.erase(function->GetId());
		Singleton().functionsByPtr.erase(function->GetPtr());
	}
	
	void FunctionRegistry::Add(FunctionBase* function) {
		Singleton().functions[function->GetId()] = function;
		Singleton().functionsByPtr[function->GetPtr()] = function;
	}
	
	FunctionBase* FunctionRegistry::GetById(uint32_t id) {
		auto it = Singleton().functions.find(id);
		if(it != Singleton().functions.end())
			return it->second;
		return NULL;
	}
	
	FunctionBase* FunctionRegistry::GetByPtr(void* ptr) {
		auto it = Singleton().functionsByPtr.find(ptr);
		if(it != Singleton().functionsByPtr.end())
			return it->second;
		return NULL;
	}
	
	
	void FunctionRegistry::Call(serialization::Reader& args) {
		uint32_t functionId;
		args >> functionId;
		FunctionBase* function = GetById(functionId);
		if(function) {
			function->Execute(args);
		}
	}
}

