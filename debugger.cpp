#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "breakpoint.hpp"
using namespace std;
#include <sys/user.h>
#include <map>

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

map<uint64_t, uint64_t> originalInst;
bool continue_execution(int pid)
{
	// Lets continue execution
	ptrace(PTRACE_CONT, pid, 0, 0);
	wait(NULL);
	
	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS, pid, NULL, &regs);

	// Go back one instruction and write the original instruction
	regs.rip -= 1;
	printf("Break at : %llx , Continuing...\n", regs.rip);
	if(originalInst.find(regs.rip) == originalInst.end())
	{
		cout << "No BreakPoint set after this exiting main \n";return false;
	}
	ptrace(PTRACE_SETREGS, pid, NULL, &regs);

	// Replace the original instruction
	breakpoint::unset_breakpoint(pid, regs.rip, originalInst[regs.rip]);	
	return true;
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

		if(iaddr == 1)
		{
			if(!continue_execution(pid))return;	
		}
		else
		{
			// Save the original instruction data
			originalInst[iaddr] = breakpoint::set_breakpoint(pid, iaddr);
		}
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
