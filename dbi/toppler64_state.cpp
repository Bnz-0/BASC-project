#include "pin.H"
#include <fstream>
#include <iostream>

using namespace std;

ostream *out;
KNOB<string> knob_outfile(KNOB_MODE_WRITEONCE, "pintool", "o", "/dev/stderr", "output filename");

const char *states[] = {"standing", "jumping", "falling", "turning", "door", "shooting", "elevator", "toppling", "drown", "drowned", "finished"};
void *toppler64_state_address=reinterpret_cast<void*>(0x46cd40);


VOID readState (ADDRINT writeAddress) {
    if (writeAddress == (ADDRINT)toppler64_state_address) {
		UINT8 i;
		PIN_SafeCopy(&i, (VOID*) (toppler64_state_address), sizeof(UINT8));
		*out << (i < 11 ? states[i] : "unknown") << endl;
    }
}

VOID onInstructionLoad(INS ins, VOID* v) {
	if (INS_IsMemoryWrite(ins) && INS_IsValidForIpointAfter(ins)) {
		int opCount = INS_MemoryOperandCount(ins);
		for(int i=0; i<opCount; i++) {
			if(INS_MemoryOperandIsWritten(ins, i)) {
				INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)readState, IARG_MEMORYOP_EA, i, IARG_END);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	if (PIN_Init(argc, argv))
		return 1;
	out = new ofstream(knob_outfile.Value().c_str());

	INS_AddInstrumentFunction(onInstructionLoad, NULL);

	PIN_StartProgram();
}
