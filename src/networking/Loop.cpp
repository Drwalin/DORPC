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

#include <libusockets.h>

#include "Loop.hpp"

#include "../Debug.hpp"

namespace net {
	Loop*& Loop::ThisThreadLoop() {
		DEBUG("");
		static thread_local Loop* thread_loop = NULL;
		return thread_loop;
	}
	
	void Loop::InternalDestructor() {
		DEBUG("");
		delete events;
		events = NULL;
		delete contexts;
		contexts = NULL;
		us_loop_free(loop);
	}

	void Loop::Run() {
		DEBUG("");
		if(ThisThreadLoop() != NULL)
			throw "Cannot run loop on thread with already running loop.";
		running = true;
		ThisThreadLoop() = this;
		us_loop_run(loop);
		ThisThreadLoop() = NULL;
		running = false;
	}

	void Loop::PushEvent(Event* event) {
		DEBUG("");
		event->defer = 0;
		events->push(event);
		us_wakeup_loop(loop);
	}

	void Loop::DeferEvent(int defer, Event* event) {
		DEBUG("");
		event->defer = defer;
		events->push(event);
		us_wakeup_loop(loop);
	}

	void Loop::PopEvents() {
		DEBUG("");
		Event* event;
		std::vector<Event*> deferedEvents;
		while((event = events->pop()) != NULL) {
		DEBUG("");
			if(event->defer > 0) {
		DEBUG("DEFERED");
				deferedEvents.emplace_back(event);
			} else {
		DEBUG("");
				if(event)
					event->Run();
				else
					break;
				Event::Free(event);
			}
		}
		for(size_t i=0; i<deferedEvents.size(); ++i) {
		DEBUG("");
			events->push(deferedEvents[i]);
		}
		deferedEvents.clear();
	}

	void Loop::OnWakeup() {
		DEBUG("");
		PopEvents();
	}

	void Loop::OnPre() {
		DEBUG("");
		PopEvents();
	}

	void Loop::OnPost() {
		DEBUG("");
		PopEvents();
	}

	void Loop::InternalOnWakeup(struct us_loop_t* loop) {
		DEBUG("");
		((Loop*)us_loop_ext(loop))->OnWakeup();
	}

	void Loop::InternalOnPre(struct us_loop_t* loop) {
		DEBUG("");
		((Loop*)us_loop_ext(loop))->OnPre();
	}

	void Loop::InternalOnPost(struct us_loop_t* loop) {
		DEBUG("");
		((Loop*)us_loop_ext(loop))->OnPost();
	}

	Loop* Loop::Make() {
		DEBUG("");
		struct us_loop_t* us_loop = us_create_loop(0, InternalOnWakeup,
				InternalOnPre, InternalOnPost, sizeof(Loop));
		Loop* loop = (Loop*)us_loop_ext(us_loop);
		loop->loop = us_loop;
		loop->userData = NULL;
		loop->running = false;
		loop->events = new concurrent::mpsc::queue<Event>();
		loop->contexts = new std::set<Context*>();
		return loop;
	}
}

