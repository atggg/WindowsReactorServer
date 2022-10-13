#include"TcpServer.h"

int main()
{
	TcpServer tcp(19999,4);
	tcp.run();
	return 0;
}