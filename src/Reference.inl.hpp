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

#ifndef REFERENCE_INL_HPP
#define REFERENCE_INL_HPP

#include "Reference.hpp"
#include "Cluster.hpp"

namespace impl {
	template<class T, typename Ret, typename... Args>
	struct Call {
		Reference<T> ref;
		Ret (T::*method)(Args...);
		inline void Do(Args... args) {
			Cluster::Singleton()->Call<T, Ret, Args...>(ref, method, args...);
		}
	};

	template<class T, typename Ret, typename... Args>
	inline Call<T, Ret, Args...> MakeCall(Reference<T> ref,
			Ret (T::*method)(Args...)) {
		return Call<T, Ret, Args...>{ref, method};
	}
}

#endif

