
#include <iostream>
#include <ostream>
#include <chrono>

#include <rpc/FunctionRegistry.hpp>
#include <rpc/Function.hpp>
#include <rpc/Context.hpp>

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
	int ret = c + (int64_t)(a*b) + 13;
	validate(id, ret == res);
	return ret;
}

std::string functionB(int id, std::string res, std::string a,
		std::vector<uint32_t> b, std::vector<std::string> c) {
	a = std::string("<begin>") + a;
	for(int i : b) {
		a += ".." + c[i%c.size()];
	}
	a += "<end>";
	validate(id, a == res);
	return a;
}

#define TEST(F, NODE, ID, R, ...) RPC(F, NODE, ID, F(0, R, __VA_ARGS__), \
		__VA_ARGS__)

struct us_listen_socket_t* listenSocket = NULL;

int main(int argc, char** argv) {
	bool _c = true;
	if(argc != 2) {
		_c = false;
	} else {
		if(strcmp(argv[1], "server")==0 || strcmp(argv[1], "client")==0)
			_c = false;
	}
	if(_c) {
		printf(" Invalid arguments ... FAILED\n\n");
		return 1;
	}
	
	REGISTER_FUNCTION(functionA);
	REGISTER_FUNCTION(functionB);
	
	rpc::Context context([](net::Socket* socket, rpc::Context* context,
			bool isClient, std::string ip) {
				TEST(functionA, 1, 1, 0, 'a', 'b', 'c');
				TEST(functionA, 1, 2, 0, 'd', 'e', 'f');
				TEST(functionA, 1, 3, 0, 'g', 'h', 'i');
				TEST(functionA, 1, 4, 0, 'j', 'k', 'l');
				TEST(functionA, 1, 5, 0, 'm', 'n', 'o');
				TEST(functionA, 1, 6, 0, 'r', 'q', 'p');
				TEST(functionB, 1, 7, "", "AA:", {1, 1, 0}, {"__0", "__1"});
				TEST(functionB, 1, 8, "", "_B:", {1, 1, 0}, {"__3", "__2"});
				TEST(functionB, 1, 9, "", "C_:", {1, 2, 0}, {"__4", "__5", "_6"});
				TEST(functionB, 1, 10, "", "++:", {1, 2, 0}, {"_9", "_10", "_11"});
			},
			[](net::Socket* socket, rpc::Context* context, int, void*) {
				socket->InternalClose();
				exit(1);
			},
			NULL,
			"cert/user.key", "cert/user.crt", "cert/rootca.crt", NULL);
	
	if(strcmp(argv[1], "server") == 0) {
		context.InternalListen("127.0.0.1", 12345);
	} else {
		context.InternalConnect("127.0.0.1", 12345);
	}
	
	std::thread thread([](rpc::Context& context) { context.Run(); });
	
	auto end = std::chrono::high_resolution_clock::now() +
		std::chrono::seconds(2);
	while(total != 10 && end>std::chrono::high_resolution_clock::now()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	
	if(invalid+valid != 10) {
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
	
	return invalid;
}

