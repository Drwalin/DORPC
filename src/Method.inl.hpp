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

#ifndef DORPC_METHOD_INL_HPP
#define DORPC_METHOD_INL_HPP

#include <tuple>

#include "Util.hpp"
#include "Method.hpp"

namespace impl {
	template<typename T, typename Function, typename Tuple, size_t ... I>
	inline auto InvokeMethod(T* object, Function f, Tuple& t,
			std::index_sequence<I ...>) {
		 return (object->f)(std::get<I>(t) ...);
	}

	template<typename T, typename Function, typename Tuple>
	inline auto InvokeMethod(T* object, Function f, Tuple& t) {
		static constexpr auto size = std::tuple_size<Tuple>::value;
		return InvokeMethod(object, f, t, std::make_index_sequence<size>{});
	}
}

template<typename T, typename... Args>
class Method : public MethodBase {
public:
	
	virtual ~Method() override;
	
	using Type = void(T::*)(Args...);
	
	Method(void(T::*method)(Args...), uint64_t id, const std::string& name) :
			MethodBase(method, id, name) {}
	
	
	virtual void Add() override;
	virtual void Update() override;
	
	inline Type GetMethod() const {return (Type)methodPtr;}

	virtual bool Execute(void* objectPtr, Buffer* buffer) const override {
		std::tuple<Args...> arguments;
		if(Util::Unpack(arguments, buffer) == false)
			return false;
		try {
			impl::InvokeMethod((T*)objectPtr, GetMethod(), arguments);
		} catch (...) {
			return false;
		}
		return true;
	}
};

#endif

