
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

int functionC(std::string str) {
	printf(" Received (functionC): %s\n\n", str.c_str());
	return (int)str.size();
}

#define TEST(F, NODE, ID, R, ...) RPC(F, NODE, ID, F(0, R, __VA_ARGS__), \
		__VA_ARGS__)

struct us_listen_socket_t* listenSocket = NULL;

int PORT = 32515;

int main(int argc, char** argv) {
	ARGV[1] = (char*)"server";
	
	srand(time(NULL));
	
	PORT = 2000 + (rand()%60000);
	
	REGISTER_FUNCTION(functionA);
	REGISTER_FUNCTION(functionB);
	REGISTER_FUNCTION(functionC);
	
	rpc::Context context([](std::shared_ptr<net::Socket> socket, rpc::Context* context,
			bool isClient, std::string ip) {
				uint32_t id = socket->userData32;
				TEST(functionA, id, 1, 0, 'a', 'b', 'c');
				TEST(functionA, id, 2, 0, 'd', 'e', 'f');
				TEST(functionA, id, 3, 0, 'g', 'h'*3.12341f, 'i');
				TEST(functionA, id, 4, 0, 'j', 'k', 'l');
				TEST(functionA, id, 5, 0, 'm', 'n', 'o');
				TEST(functionA, id, 6, 0, 'r', 'q', 'p');
				TEST(functionB, id, 7, "", "AA:", {1, 1, 0}, {"__0", "__1"});
				TEST(functionB, id, 8, "", "_B:", {1, 1, 0}, {"__3", "__2"});
				TEST(functionB, id, 9, "", "C_:", {1, 2, 0}, {"__4", "__5", "_6"});
				TEST(functionB, id, 10, "", "++:", {1, 2, 0}, {"_9", "_10", "_11"});
			},
			[](std::shared_ptr<net::Socket> socket, rpc::Context* context, int, void*) {
				socket->InternalClose();
				exit(1);
			},
			[](net::Event&e){
			},
			"cert/user.key", "cert/user.crt", "cert/rootca.crt", NULL);
	
	context.InternalListen("127.0.0.1", PORT);
	int count = 1;//507;
	for(int i=0; i<count; ++i)
		context.Connect("127.0.0.1", PORT);
	
	context.AsyncRun();
	
	total = count*20;
	
 	//std::this_thread::sleep_for(std::chrono::milliseconds(30));
	auto end = std::chrono::high_resolution_clock::now() +
		std::chrono::seconds(20);
	while(total != (valid+invalid)
			&& end>std::chrono::high_resolution_clock::now()
			&& context.IsRunning()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	
	if(invalid+valid != 20) {
		printf(" tests valid %i + invalid %i / total %i\n\n", valid.load(),
				invalid.load(), total.load());
	} else {
		if(invalid) {
			printf(" tests %i/%i ... FAILED\n\n", invalid.load(), total.load());
		} else {
			printf(" tests %i/%i ... OK\n\n", valid.load(), total.load());
		}
	}
	fflush(stderr);
	fflush(stdout);
	
	
	exit(invalid);
	
	return invalid;
}

