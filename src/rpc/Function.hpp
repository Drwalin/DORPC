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

#ifndef DORPC_RPC_FUNCTION_HPP
#define DORPC_RPC_FUNCTION_HPP

#include <unordered_map>

#include "FunctionBase.hpp"
#include "FunctionRegistry.hpp"

namespace rpc {
	
	template<typename T>
	struct FunctionTraits;
	
	template<typename Ret, typename... Args>
	struct FunctionTraits<Ret(*)(Args...)> {
		using tuple = std::tuple<Args...>;
		using ret = Ret;
		using type = std::function<Ret(Args...)>;
		inline static tuple MakeTuple(Args... args) {
			return std::make_tuple(args...);
		}
	};
	
	template<typename Type, Type ptr>
	class Function : public FunctionBase {
	public:
		
		inline virtual void* GetPtr() override {
			return (void*)ptr;
		}
		
		inline static void* StaticGetPtr() {
			return (void*)ptr;
		}
		
		virtual ~Function() override {
			FunctionRegistry::Remove(this);
		}
		
		inline static void Register() {
			FunctionRegistry::Add(new Function<Type, ptr>());
		}
		
		inline static FunctionBase* Instance() {
			static FunctionBase* instance =
				FunctionRegistry::Singleton().GetByPtr(StaticGetPtr());
			return instance;
		}
		
		virtual void Execute(serialization::Reader& reader) override {
			typename FunctionTraits<Type>::tuple args;
			reader >> args;
			std::apply(ptr, args);
		}
		
		virtual void ExecuteWithReturn(serialization::Reader& reader,
				serialization::Writer& writerRet) override {
			typename FunctionTraits<Type>::tuple args;
			reader >> args;
			writerRet << std::apply(ptr, args);
		}
		
	protected:
		
		Function() = default;
	};
	
	template<typename Type, Type func>
	inline void RegisterFunction() {
		Function<Type, func>::Register();
	}
}

#define FUNCTION(__F) rpc::Function<decltype(__F), __F>::Instance()
#define REGISTER_FUNCTION(__F) rpc::Function<decltype(&__F), __F>::Register()

#endif

