
#include <networking/Context.hpp>
#include <networking/Loop.hpp>
#include <networking/Socket.hpp>

#include <thread>
#include <cstring>

const uint16_t ports[2] = {12345, 12346};

void process(int portOpen, int portOther, int id) {
	Loop *loop = Loop::Make();
	Context* context = Context::Make(loop, [=](Socket*socket,
				int a, char* b, int c) {
				Buffer buffer;
				char str[1024];
				sprintf(str, "Hello from %i to %i, has been sent", portOpen, portOther);
				buffer.Write(str, strlen(str)+1);
				socket->Send(buffer);
			},
			[=](Buffer&buffer, Socket*socket){
				printf(" Received (on %i): '%s'\n", portOpen, buffer.Data());
			}, "cert/user.key",
			"cert/user.crt", "cert/rootca.crt", NULL);
	
	context->StartListening("127.0.0.1", portOpen);
	Socket* socket = context->InternalConnect("127.0.0.1", portOther);
	loop->Run();
}

int main() {
	std::thread thread = std::thread(process, ports[0], ports[1], 0);
	process(ports[1], ports[0], 1);
	
	thread.join();
	return 0;
}

