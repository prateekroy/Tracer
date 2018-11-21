#include <stdint.h>
#include <sys/ptrace.h>
#include <stdint.h>
#include <iostream>
#include <stdio.h>
using namespace std;

class breakpoint{
		public:
		uint64_t pid; uint64_t addr;
		uint64_t orig_data;
		breakpoint(){}
		breakpoint(uint64_t _pid, uint64_t _addr){pid = _pid; addr = _addr;}
		void set_breakpoint(){
			orig_data = ptrace(PTRACE_PEEKTEXT, pid, addr, 0);			
			printf("Address : %lx Orig Data : %lx\n", addr, orig_data);
			uint64_t new_data = (orig_data&~0xFF)|0xCC; 		//Insert a int3 = 0xcc
			printf("New Data : %lx\n", new_data);
			ptrace(PTRACE_POKETEXT, pid, addr, new_data);
		}

		void unset_breakpoint(uint64_t in_addr, uint64_t in_data)
		{
			//uint64_t data = ptrace(PTRACE_PEEKTEXT, pid, addr, 0);
			printf("New Data : %lx \n", in_data);
            uint64_t new_data = in_data;         //Insert a int3 = 0xcc
			printf("Restore Address : %ld Orig Data : %lx\n", in_addr, new_data);
            ptrace(PTRACE_POKETEXT, pid, in_addr, new_data);			
		}
};
