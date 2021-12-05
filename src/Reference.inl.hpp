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

#ifndef DORPC_REFERENCE_INL_HPP
#define DORPC_REFERENCE_INL_HPP

#include "Reference.hpp"
#include "Cluster.hpp"

namespace impl {
	template<class T, typename... Args>
	struct Invoke {
		Reference<T> ref;
		void(T::*method)(Args...);
		inline void Do(Args... args) {
			Cluster::Singleton()->Call<T, Args...>(ref, method, args...);
		}
	};

	template<class T, typename... Args>
	inline Invoke<T, Args...> MakeInvoke(Reference<T> ref,
			void(T::*method)(Args...)) {
		return Invoke<T, Args...>{ref, method};
	}
	
	
	template<typename... Args>
	struct Call {
		void(*function)(Args...);
		inline void Do(Args... args) {
			Cluster::Singleton()->Call<Args...>(function, args...);
		}
	};

	template<typename... Args>
	inline Call<Args...> MakeCall(void(*function)(Args...)) {
		return Call<Args...>{function};
	}
}

#endif

