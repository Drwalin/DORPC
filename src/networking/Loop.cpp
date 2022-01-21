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

namespace networking {
	void Loop::InternalDestructor() {
		delete events;
		events = NULL;
		delete contexts;
		contexts = NULL;
		us_loop_free(loop);
	}

	void Loop::Run() {
		us_loop_run(loop);
	}

	void Loop::PushEvent(Event* event) {
		events->push(event);
		us_wakeup_loop(loop);
	}

	void Loop::PopEvents() {
		Event* event;
		while((event = events->pop()) != NULL) {
			if(event)
				event->Run();
			else
				break;
		}
	}

	void Loop::OnWakeup() {
		PopEvents();
	}

	void Loop::OnPre() {
		PopEvents();
	}

	void Loop::OnPost() {
		PopEvents();
	}

	void Loop::InternalOnWakeup(struct us_loop_t* loop) {
		((Loop*)us_loop_ext(loop))->OnWakeup();
	}

	void Loop::InternalOnPre(struct us_loop_t* loop) {
		((Loop*)us_loop_ext(loop))->OnPre();
	}

	void Loop::InternalOnPost(struct us_loop_t* loop) {
		((Loop*)us_loop_ext(loop))->OnPost();
	}

	Loop* Loop::Make() {
		struct us_loop_t* us_loop = us_create_loop(0, InternalOnWakeup,
				InternalOnPre, InternalOnPost, sizeof(Loop));
		Loop* loop = (Loop*)us_loop_ext(us_loop);
		loop->loop = us_loop;
		loop->userData = NULL;
		loop->events = new concurrent::mpsc::queue<Event>();
		loop->contexts = new std::set<Context*>();
		return loop;
	}
}

