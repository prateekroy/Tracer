#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;
#include <sys/user.h>
#include <map>
#include "utilities.hpp"
#include "breakpoint.hpp"

void run_target(string prog)
{
	if(ptrace(PTRACE_TRACEME, 0, 0, 0) < 0)
	{
		cout << "Cannot trace" << endl;
		return;
	}

	execl(prog.c_str(), prog.c_str(), NULL);
}

void printallregs(int pid)
{
	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS, pid, NULL, &regs);
	printf(" rax : %llx, rbx : %llx, rcx : %llx, rdx : %llx, rsi : %llx, rdi : %llx, rip : %llx, rsp : %llx\n", regs.rax, regs.rbx, regs.rcx, regs.rdx, regs.rsi, regs.rdi, regs.rip, regs.rsp);
	printf(" r8 : %llx, r9 : %llx, r10 : %llx, r11 : %llx, r12 : %llx, r13 : %llx, r14 : %llx, r15 : %llx\n", regs.r8, regs.r9, regs.r10, regs.r11, regs.r12, regs.r13, regs.r14, regs.r15);
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
		string command; 
		std::getline(std::cin, command);
		vector<string> elems;
		split(command, ' ', std::back_inserter(elems));	
		if(elems[0] == "break" || elems[0] == "b")
		{	
			// Save the original instruction data
			int iaddr = atox(elems[1].c_str());
		    originalInst[iaddr] = breakpoint::set_breakpoint(pid, iaddr);
		}
		else if(elems[0] == "cont" || elems[0] == "c")
		{
			if(!continue_execution(pid))return;
		}
		else if(elems[0] == "printall" || elems[0] == "p")
		{
			printallregs(pid);
		}		
		else
		{
			return;
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
