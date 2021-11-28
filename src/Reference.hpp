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

#ifndef DORPC_REFERENCE_HPP
#define DORPC_REFERENCE_HPP

#include <cinttypes>

template<class T>
struct Reference {
public:
	
	using Type = T;
	
	Reference() : id(0) {}
	Reference(const Reference&) = default;
	Reference(Reference&) = default;
	Reference(Reference&&) = default;
	~Reference() {}
	
	inline Reference& operator=(const Reference&) = default;
	inline Reference& operator=(Reference&) = default;
	inline Reference& operator=(Reference&&) = default;
	
	inline uint64_t Get() const { return id; }
	
private:
	
	uint64_t id;
};

#include "Reference.inl.hpp"

#define RPC(OBJECT_REF, METHOD, ...) \
		(impl::MakeCall(OBJECT_REF, \
			decltype(OBJECT_REF)::Type::METHOD) \
			.Do(__VA_ARGS__))

#endif

