
#include <networking/Context.hpp>
#include <networking/Loop.hpp>
#include <networking/Socket.hpp>

#include <thread>
#include <cstring>
#include <string_view>

const uint16_t ports[2] = {12345, 12346};

std::atomic<int> received_counter = 0;

void process(int portOpen, int portOther, int id) {
	networking::Loop *loop = networking::Loop::Make();
	networking::Context* context = networking::Context::Make(loop, [=](
				networking::Socket*socket, int a, char* b, int c) {
			
				networking::Buffer buffer;
				char str[1024];
				sprintf(str, "Hello from %i to %i, has been sent", portOpen,
						portOther);
				buffer.Write(str, strlen(str)+1);
				socket->Send(buffer);
			},
			[=](networking::Buffer& buffer, networking::Socket* socket) {
				std::string_view v((char*)buffer.Data(), buffer.Size()-1);
				bool valid = v.starts_with("Hello from ")
						&& v.ends_with(", has been sent");
				printf(" Received (on %i of size %i) [%c;%c]: '%s': %s\n",
						portOpen, buffer.Size(), v[0], v[v.size()-1],
						buffer.Data(), valid?"valid":"ERROR!!!");
				if(valid == false) {
					std::this_thread::yield();
					exit(1);
				} else {
					received_counter++;
					if(received_counter == 4) {
						printf(" done\n");
						printf(" no errors: 4/4 ... OK\n");
						exit(0);
					}
				}
			}, "cert/user.key", "cert/user.crt", "cert/rootca.crt", NULL);
	
	context->StartListening("127.0.0.1", portOpen);
	networking::Socket* socket_ = context->InternalConnect("127.0.0.1",
			portOther);
	socket_->userData = NULL;
	loop->Run();
}

int main() {
	std::thread thread = std::thread(process, ports[0], ports[1], 0);
	process(ports[1], ports[0], 1);
	
	thread.join();
	return 0;
}

