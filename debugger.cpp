#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "breakpoint.hpp"
using namespace std;

// Utilities
int atox(const char *s)
{
    int x= 0;
    while (*s) {
        x= x*16+(*s>'9'?(toupper(*s)-'A'+10):*s-'0');
        s++;
    }
    return x;
}

void run_target(string prog)
{
	if(ptrace(PTRACE_TRACEME, 0, 0, 0) < 0)
	{
		cout << "Cannot trace" << endl;
		return;
	}

	execl(prog.c_str(), prog.c_str(), NULL);
}

void run_debugger(int pid)
{
	wait(NULL);
	
	while(1)
	{
		cout << "\n>";
		string breakaddr; cin >> breakaddr;
		cout << breakaddr << endl;
		int iaddr = atox(breakaddr.c_str());
		if(iaddr == 0)break;		

		breakpoint bp(pid, iaddr);
		bp.set_breakpoint();
		cout << "Set breakpoint\n";
		ptrace(PTRACE_CONT, pid, 0, 0);	
		wait(NULL);
	}
}

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		cout << "Please give executable to debug!" << endl;
		return 0;
	}

	int child_pid = fork();
	if(child_pid == 0)
	{
		//child	
		run_target(argv[1]);
	}
	else 
	{
		//parent
		run_debugger(child_pid);
//		wait(&child_pid);
	}
}
