#include <stdint.h>
#include <signal.h>
#include <map>
#include <list>
#include <memory>
#include <unistd.h>
#include "MyBlePeripheral.h"

using namespace std;

static volatile int caught_sigint = 0;

void sigint_handler(int sig)
{
	caught_sigint = sig;
}


int main()
{
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_NOCLDSTOP;
	sa.sa_handler = sigint_handler;
	sigaction(SIGINT, &sa, NULL);

	{
		MyBlePeripheral peripheral;

		printf("Initializing BLE peripheral...");
		if (peripheral.init()) return -1;
		printf("ok\n");

		while ((caught_sigint == 0) && (getchar() != 'x'))
		{
			sleep(1);
		}
		peripheral.cleanup();


		//MyBleCentral scanner;

		//printf("Initializing BLE central...");
		//if (scanner.init()) return -1;
		//printf("ok\n");

		//while ((caught_sigint == 0) && (getchar() != 'x'))
		//{
		//	sleep(1);
		//}
		//scanner.cleanup();
	}

	printf("Exited\n");
	return 0;
}
