#include <windows.h>
#include "KernelBase.h"
#include <string.h>

PVOID getKernelBase(char* moduleName)
{
	HMODULE ntdll = GetModuleHandle(TEXT("ntdll"));
	PNtQuerySystemInformation query = (PNtQuerySystemInformation)GetProcAddress(ntdll, "NtQuerySystemInformation");
	if (query == NULL) {
		printf("GetProcAddress() failed.\n");
		return 1;
	}
	ULONG len = 0;
	query(SystemModuleInformation, NULL, 0, &len);

	PSYSTEM_MODULE_INFORMATION pModuleInfo = (PSYSTEM_MODULE_INFORMATION)GlobalAlloc(GMEM_ZEROINIT, len);
	if (pModuleInfo == NULL) {
		printf("Could not allocate memory for module info.\n");
		return 1;
	}
	NTSTATUS status = query(SystemModuleInformation, pModuleInfo, len, &len);
	

	if (status != (NTSTATUS)0x0) {
		printf("NtQuerySystemInformation failed with error code 0x%X\n", status);
		return 1;
	}
	for (unsigned int i = 0; i < pModuleInfo->ModulesCount; i++) {
		PVOID kernelImageBase = pModuleInfo->Modules[i].ImageBaseAddress;
		PCHAR kernelImage = (PCHAR)pModuleInfo->Modules[i].Name;
		int pathlen = strlen(kernelImage), len = strlen(moduleName);

		for (int j = 0; j < pathlen - len; j++) {
			if (!strncmp(moduleName, kernelImage+j, len)) {
				printf("Mod name %s ", kernelImage);
		#ifdef _WIN64
				printf("Base Addr 0x%llx\r\n", kernelImageBase);
		#else
				printf("Base Addr 0x%X\r\n", kernelImageBase);
		#endif
				return kernelImageBase;
			}
		}


	}
	return NULL;
}