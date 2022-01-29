/*
 *  This file is part of DORPC. Please see README for details.
 *  Copyright (C) 2021-2022 Marek Zalewski aka Drwalin
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

#ifndef DORPC_NETWORKING_LOOP_HPP
#define DORPC_NETWORKING_LOOP_HPP

#include <mpsc_queue.hpp>
#include <set>

#include "Event.hpp"

namespace networking {
	struct Loop {
		struct us_loop_t* loop;
		void * userData;
		bool running;

		concurrent::mpsc::queue<Event> *events;
		std::set<Context*> *contexts;
		
		
		static Loop*& ThisThreadLoop();
		
		void InternalDestructor();

		void Run();


		void PushEvent(Event* event);
		void DeferEvent(int defer, Event* event);
		void PopEvents();

		void OnWakeup();
		void OnPre();
		void OnPost();

		static void InternalOnWakeup(struct us_loop_t* loop);
		static void InternalOnPre(struct us_loop_t* loop);
		static void InternalOnPost(struct us_loop_t* loop);

		static Loop* Make();
	};
}

#endif

