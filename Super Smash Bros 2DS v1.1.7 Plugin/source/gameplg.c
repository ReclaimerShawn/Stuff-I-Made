#include "global.h"

#define WRITEU8(addr, data) *(vu8*)(addr) = data
#define WRITEU16(addr, data) *(vu16*)(addr) = data
#define WRITEU32(addr, data) *(vu32*)(addr) = data
#define READU8(addr) *(volatile unsigned char*)(addr)
#define READU16(addr) *(volatile unsigned short*)(addr)
#define READU32(addr) *(volatile unsigned int*)(addr)

u32 threadStack[0x1000];
Handle fsUserHandle;
FS_archive sdmcArchive;


#define IO_BASE_PAD		1
#define IO_BASE_LCD		2
#define IO_BASE_PDC		3
#define IO_BASE_GSPHEAP		4

u32 IoBasePad = 0xFFFD4000;

u32 getKey() {
	return (*(vu32*)(IoBasePad) ^ 0xFFF) & 0xFFF;
}

void waitKeyUp() {
	while (getKey() != 0) {
		svc_sleepThread(100000000);
	}
}

u8 cheatEnabled[64];
int isNewNtr = 0;


u32 plgGetIoBase(u32 IoType);
GAME_PLUGIN_MENU gamePluginMenu;

void initMenu() {
	memset(&gamePluginMenu, 0, sizeof(GAME_PLUGIN_MENU));
}

void addMenuEntry(u8* str) {
	if (gamePluginMenu.count > 64) {
		return;
	}
	u32 pos = gamePluginMenu.count;
	u32 len = strlen(str) + 1;
	gamePluginMenu.offsetInBuffer[pos] = gamePluginMenu.bufOffset;
	strcpy(&(gamePluginMenu.buf[gamePluginMenu.bufOffset]), str);
	
	gamePluginMenu.count += 1;
	gamePluginMenu.bufOffset += len;
}

u32 updateMenu() {
	PLGLOADER_INFO *plgLoaderInfo = (void*)0x07000000;
	plgLoaderInfo->gamePluginPid = getCurrentProcessId();
	plgLoaderInfo->gamePluginMenuAddr = (u32)&gamePluginMenu;
	
	u32 ret = 0;
	u32 hProcess;
	u32 homeMenuPid = plgGetIoBase(5);
	if (homeMenuPid == 0) {
		return 1;
	}
	ret = svc_openProcess(&hProcess, homeMenuPid);
	if (ret != 0) {
		return ret;
	}
	copyRemoteMemory( hProcess, &(plgLoaderInfo->gamePluginPid), CURRENT_PROCESS_HANDLE,  &(plgLoaderInfo->gamePluginPid), 8);
	final:
	svc_closeHandle(hProcess);
	return ret;
}

int scanMenu() {
	u32 i;
	for (i = 0; i < gamePluginMenu.count; i++) {
		if (gamePluginMenu.state[i]) {
			gamePluginMenu.state[i] = 0;
			return i;
		}
	}
	return -1;
}

// detect language (0: english)
int detectLanguage() {
	// unimplemented
	return 0;
}

// add one cheat menu entry
void addCheatMenuEntry(u8* str) {
	u8 buf[100];
	xsprintf(buf, "[ ] %s", str);
	addMenuEntry(buf);
}

// this function will be called when the state of cheat item changed
void onCheatItemChanged(int id, int enable) {
	// TODO: handle on cheat item is select or unselected
	

}

// freeze the value
void freezeCheatValue() {
	if (cheatEnabled[0]) {
		unsigned int myhp = READU32(0x0804866C) + 0x1B0C;
		WRITEU32(myhp, 0x00000000);
	}
	
	if (cheatEnabled[1]) {
		unsigned int enem1hp = READU32(0x08049B94) + 0x1B0C;	
		WRITEU32(enem1hp, 0x00000000);
	}
	
	if (cheatEnabled[2]) {
		unsigned int enem2hp = READU32(0x0804B0BC) + 0x1B0C;
		WRITEU32(enem2hp, 0x00000000);
	}
	
	if (cheatEnabled[3]) {
		unsigned int enem3hp = READU32(0x0804C5E4) + 0x1B0C;
		WRITEU32(enem3hp, 0x00000000);
	}

	if (cheatEnabled[4]) {
		unsigned int myhp = READU32(0x0804866C) + 0x1B0C;
		WRITEU32(myhp, 0x4479C000);
	}
	
	if (cheatEnabled[5]) {
		unsigned int enem1hp = READU32(0x08049B94) + 0x1B0C;	
		WRITEU32(enem1hp, 0x4479C000);
	}
	
	if (cheatEnabled[6]) {
		unsigned int enem2hp = READU32(0x0804B0BC) + 0x1B0C;
		WRITEU32(enem2hp, 0x4479C000);
	}
	
	if (cheatEnabled[7]) {
		unsigned int enem3hp = READU32(0x0804C5E4) + 0x1B0C;
		WRITEU32(enem3hp, 0x4479C000);
	}
	
	if (cheatEnabled[8]) {
		unsigned int mylife = READU32(0x08047724) + 0x1AC;
		WRITEU8(mylife, 0x63);
	}
	
	if (cheatEnabled[9]) {
		unsigned int myhp1p = READU32(0x0804B4CC) + 0x1B0C;
		WRITEU32(myhp1p, 0x00000000);
	}

	if (cheatEnabled[10]) {
		unsigned int myhp1pmii = READU32(0x08053144) + 0x1B0C;
		WRITEU32(myhp1pmii, 0x00000000);
	}

	if (cheatEnabled[11]) {
		unsigned int myhp1pmh = READU32(0x0804B10C) + 0x1B0C;
		WRITEU32(myhp1pmh, 0x00000000);
	}

	if (cheatEnabled[12]) {
		unsigned int myhprun = READU32(0x0804FA40) + 0x1B0C;
		WRITEU32(myhprun, 0x00000000);
	}

	if (cheatEnabled[13]) {
		unsigned int enem1hp1p = READU32(0x0804C9F4) + 0x1B0C;
		WRITEU32(enem1hp1p, 0x4479C000);
	}
	
	if (cheatEnabled[14]) {
		unsigned int enem2hp1p = READU32(0x0804DF1C) + 0x1B0C;
		WRITEU32(enem2hp1p, 0x4479C000);
	}

	if (cheatEnabled[15]) {
		unsigned int enem3hp1p = READU32(0x0804F444) + 0x1B0C;
		WRITEU32(enem3hp1p, 0x4479C000);
	}

	if (cheatEnabled[16]) {
		WRITEU8(0x00CA9F0E, 0x49);
		WRITEU8(0x00CA9F13, 0x49);
	}

	if (cheatEnabled[17]) {
		WRITEU8(0x00CA9F0E, 0x4A);
		WRITEU8(0x00CA9F13, 0x4A);
	}

	if (cheatEnabled[18]) {
		WRITEU8(0x00CA9F0E, 0x4B);
		WRITEU8(0x00CA9F13, 0x4B);
	}

	// TODO: handle your own cheat items
}

// update the menu status
void updateCheatEnableDisplay(id) {
	gamePluginMenu.buf[gamePluginMenu.offsetInBuffer[id] + 1] = cheatEnabled[id] ? 'X' : ' ';
}

// scan and handle events
void scanCheatMenu() {
	int ret = scanMenu();
	if (ret != -1) {
		cheatEnabled[ret] = !cheatEnabled[ret];
		updateCheatEnableDisplay(ret);
		onCheatItemChanged(ret, cheatEnabled[ret]);
	}
}

// init 
void initCheatMenu() {
	initMenu();
	addCheatMenuEntry("You Stay Around 0% - VS Mode");
	addCheatMenuEntry("2P Stays Around 0% - VS Mode");
	addCheatMenuEntry("3P Stays Around 0% - VS Mode");
	addCheatMenuEntry("4P Stays Around 0% - VS Mode");
	addCheatMenuEntry("You At 999% - VS Mode");	
	addCheatMenuEntry("2P At 999% - VS Mode");
	addCheatMenuEntry("3P At 999% - VS Mode");
	addCheatMenuEntry("4P At 999% - VS Mode");
	addCheatMenuEntry("You Have x99 Lives - VS Mode");
	addCheatMenuEntry("You Stay Around 0% - Classic");
	addCheatMenuEntry("You Stay Around 0% - Classic MT");
	addCheatMenuEntry("You Stay Around 0% - Classic MH");
	addCheatMenuEntry("You Stay Around 0% - Smash Run");
	addCheatMenuEntry("2P At 999% - Classic Mode");
	addCheatMenuEntry("3P At 999% - Classic Mode");
	addCheatMenuEntry("4P At 999% - Classic Mode");
	addCheatMenuEntry("Be FMT Brawler");
	addCheatMenuEntry("Be FMT Swordfighter");
	addCheatMenuEntry("Be FMT Gunner");
	// TODO: Add your own menu entries
	
	updateMenu();
}

void gamePluginEntry() {
	u32 ret, key;
	INIT_SHARED_FUNC(plgGetIoBase, 8);
	INIT_SHARED_FUNC(copyRemoteMemory, 9);
	// wait for game starts up (5 seconds)
	svc_sleepThread(5000000000);

	if (((NS_CONFIG*)(NS_CONFIGURE_ADDR))->sharedFunc[8]) {
		isNewNtr = 1;
	} else {
		isNewNtr = 0;
	}
	
	if (isNewNtr) {
		IoBasePad = plgGetIoBase(IO_BASE_PAD);
	}
	initCheatMenu();
	while (1) {
		svc_sleepThread(100000000);
		scanCheatMenu();
		freezeCheatValue();
	}
}