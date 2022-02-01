
#include <iostream>
#include <ostream>
#include <chrono>

#include <rpc/FunctionRegistry.hpp>
#include <rpc/Function.hpp>
#include <rpc/Context.hpp>

#include <Debug.hpp>

std::atomic<int> valid=0, invalid=0, total=0;

void validate(int id, bool result) {
	if(id) {
		if(result) {
			valid++;
		} else {
			invalid++;
			printf(" test %i ... FAILED\n", id);
		}
		++total;
	}
}

int functionA(int id, int res, int32_t a, float b, long long c) {
	DEBUG("from A");
	int ret = c + (int64_t)(a*b) + 13;
	validate(id, ret == res);
	return ret;
}

std::string functionB(int id, std::string res, std::string a,
		std::vector<uint32_t> b, std::vector<std::string> c) {
	DEBUG("from B");
	a = std::string("<begin>") + a;
	for(int i : b) {
		a += ".." + c[i%c.size()];
	}
	a += "<end>";
	validate(id, a == res);
	return a;
}

int functionC(std::string str) {
	printf(" Received (functionC): %s\n\n", str.c_str());
	return (int)str.size();
}

#define TEST(F, NODE, ID, R, ...) RPC(F, NODE, ID, F(0, R, __VA_ARGS__), \
		__VA_ARGS__)

struct us_listen_socket_t* listenSocket = NULL;

int PORT = 32515;

int main(int argc, char** argv) {
	ARGV = argv;
	
	srand(time(NULL));
	
	PORT = 2000 + (rand()%60000);
	
	REGISTER_FUNCTION(functionA);
	REGISTER_FUNCTION(functionB);
	REGISTER_FUNCTION(functionC);
	
	rpc::Context context([](net::Socket* socket, rpc::Context* context,
			bool isClient, std::string ip) {
				printf(" onOpenSocket\n");
				uint32_t id = socket->userData32;
				TEST(functionA, id, 1, 0, 'a', 'b', 'c');
				TEST(functionA, id, 2, 0, 'd', 'e', 'f');
				TEST(functionA, id, 3, 0, 'g', 'h', 'i');
				TEST(functionA, id, 4, 0, 'j', 'k', 'l');
				TEST(functionA, id, 5, 0, 'm', 'n', 'o');
				TEST(functionA, id, 6, 0, 'r', 'q', 'p');
				TEST(functionB, id, 7, "", "AA:", {1, 1, 0}, {"__0", "__1"});
				TEST(functionB, id, 8, "", "_B:", {1, 1, 0}, {"__3", "__2"});
				TEST(functionB, id, 9, "", "C_:", {1, 2, 0}, {"__4", "__5", "_6"});
				TEST(functionB, id, 10, "", "++:", {1, 2, 0}, {"_9", "_10", "_11"});
			},
			[](net::Socket* socket, rpc::Context* context, int, void*) {
				printf(" onCloseSocket\n");
				socket->InternalClose();
				DEBUG("EXIT");
				exit(1);
			},
			[](net::Event&e){
				DEBUG(" error event: %lu, Type(%i)", e.data64, (int)e.type);
			},
			"cert/user.key", "cert/user.crt", "cert/rootca.crt", NULL);
	
	printf(" --- Listening\n\n");
	context.InternalListen("127.0.0.1", PORT);
	printf(" --- Connecting \n\n");
	context.Connect("127.0.0.1", PORT+1);
	
	printf(" --- Listening\n\n");
	context.InternalListen("127.0.0.1", PORT+1);
	printf(" --- Connecting \n\n");
	context.Connect("127.0.0.1", PORT);
	
//	std::this_thread::sleep_for(std::chrono::milliseconds(100));
//	context.Connect("127.0.0.1", PORT);
//	std::this_thread::sleep_for(std::chrono::milliseconds(100));
//	context.Connect("127.0.0.1", PORT);
//	std::this_thread::sleep_for(std::chrono::milliseconds(100));
//	context.Connect("127.0.0.1", PORT);
	
	context.AsyncRun();
	
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	auto end = std::chrono::high_resolution_clock::now() +
		std::chrono::seconds(20);
	while(total != 10 && end>std::chrono::high_resolution_clock::now()
			&& context.IsRunning()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	
	fflush(stderr);
	fflush(stdout);
	printf("\n");
	
	if(invalid+valid != 20) {
		printf(" tests [%s] valid %i + invalid %i / total %i\n\n", argv[1],
				valid.load(), invalid.load(), total.load());
	} else {
		if(invalid)
			printf(" tests [%s] %i/%i ... FAILED\n\n", argv[1], invalid.load(),
					total.load());
		else
			printf(" tests [%s] %i/%i ... OK\n\n", argv[1], valid.load(),
					total.load());
	}
	fflush(stderr);
	fflush(stdout);
	
	
	DEBUG("Legal EXIT");
	exit(invalid);
	
	return invalid;
}

