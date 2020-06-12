#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dolcesdk.h>
#include "ctrl.h"
#include "debugScreen.h"
#define printf psvDebugScreenPrintf
#define clearScreen psvDebugScreenClear

int copyFile(const char *src_path, const char *dst_path)
{
    SceUID fd_to, fd_from;
    char buf[16 * 1024];
    ssize_t nread;
    int saved_errno;
    fd_from = sceIoOpen(src_path, SCE_O_RDONLY, 0777);
    if (fd_from < 0)
        return -1;

    fd_to = sceIoOpen(dst_path, SCE_O_WRONLY | SCE_O_CREAT, 0777);
    while (nread = sceIoRead(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do
        {
            nwritten = sceIoWrite(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }

        } while (nread > 0);
    }
    if (nread == 0)
    {
        if (sceIoClose(fd_to) < 0)
        {
            fd_to = -1;
            return 0;
        }
        sceIoClose(fd_from);

        return 1;
    }
}
void increaseVolume(int vol)
{
    clearScreen();
    if (vol == 30)
    {
        volumePage();
    }
    else
    {
        int ret = sceRegMgrSetKeyInt("/CONFIG/SOUND/", "main_volume", vol + 1);
        volumePage();
    }
}

int getCurrentVolume()
{

    int val = 0;
    int buff = sceRegMgrGetKeyInt("/CONFIG/SOUND/", "main_volume", &val);
    return val;
}

void muteConsole()
{

    clearScreen(0);
    int ret = sceRegMgrSetKeyInt("/CONFIG/SOUND/", "main_volume", 0);
    volumePage();
}

void databasePage()
{

    clearScreen(0);
    sceKernelDelayThread(200000);
    printf("Press R trigger to rebuild database\n");
    printf("Press CIRCLE to go back to the main menu\n");
    while (1)
    {
        switch (get_key(0))
        {
        case SCE_CTRL_RTRIGGER:
            sceIoRemove("ux0:/id.dat");
            sceIoRemove("ur0:/shell/db/app.db");
            printf("Your console will reboot in 3 seconds.\n");
            sceKernelDelayThread(300000);
            scePowerRequestColdReset();
            break;
        case SCE_CTRL_CIRCLE:
            main();
            break;
        default:
            printf("Invalid input\n");
            sceKernelDelayThread(300000);
            main();
        }
    }
}

void decreaseVolume(int vol)
{
    clearScreen();
    if (vol == 0)
    {
        volumePage();
    }
    else
    {
        int ret = sceRegMgrSetKeyInt("/CONFIG/SOUND/", "main_volume", vol - 1);
        volumePage();
    }
}

void volumePage()
{
    clearScreen(0);
    sceKernelDelayThread(200000);
    printf("Volume:\n");
    printf("ARROW UP: increase volume.\n");
    printf("ARROW DOWN: decrease volume.\n");
    printf("TRIANGLE: mute console\n");
    printf("SQUARE: apply changes and reboot\n");
    printf("CIRCLE: go back to main menu\n");
    int currentVolume = getCurrentVolume();
    printf("Current volume %d", currentVolume);
    while (1)
    {

        switch (get_key(0))
        {
        case SCE_CTRL_UP:
            increaseVolume(currentVolume);
            break;
        case SCE_CTRL_DOWN:
            decreaseVolume(currentVolume);
            break;
        case SCE_CTRL_CIRCLE:
            main();
            break;
        case SCE_CTRL_TRIANGLE:
            muteConsole();
            break;
        case SCE_CTRL_SQUARE:
            scePowerRequestColdReset();
            break;
        default:
            printf("Invalid input\n");
            sceKernelDelayThread(300000);
            break;
        }
    }
}

void taiHenPage()
{
    clearScreen(0);
    sceKernelDelayThread(200000);

    printf("Taihen:\n");
    printf("Press CROSS to backup your tai config.\n");
    printf("Press TRIANGLE to disable plugins\n");
    printf("Press CIRLCLE to go back to the main menu\n");
    int configUr0 = 0;
    int configUx0 = 0;
    while (1)
    {
        switch (get_key(0))
        {
        case SCE_CTRL_CROSS:
            configUr0 = copyFile("ur0:/tai/config.txt", "ur0:/tai/config_ur0_backup.txt");
            configUx0 = copyFile("ux0:/tai/config.txt", "ux0:/tai/config_ux0_backup.txt");

            if (configUx0 == 1 && configUr0 == 1)
                printf("your taihen configs in both ur0 and ux0 have been backed up.\n");
            else if (configUx0 == 1 && configUr0 == -1)
                printf("Couldn't find a config in ur0\nThe one located in ux0 however, has been backed up.\n");
            else if (configUx0 == -1 && configUr0 == 1)
                printf("Couldn't find a config in ux0\nThe one located in ur0 however, has been backed up.\n");
            else
                printf("Failed to backup your configs\n");

            sceKernelDelayThread(5 * 1000000);
            taiHenPage();
            break;
        case SCE_CTRL_TRIANGLE:
                sceIoRename("ur0:/tai/", "ur0:/taixd/");
                sceIoRename("ux0:/tai/", "ux0:/taixd/");
                printf("Your console will reboot in 5 seconds.");
                sceKernelDelayThread(5 * 1000000);
                scePowerRequestColdReset();
                break;
        case SCE_CTRL_CIRCLE:
            main();
            break;
        default:
            printf("Wrong input\n");
            sceKernelDelayThread(1000000);
            taiHenPage();
            break;
        }
    }
}

void powerPage()
{
    sceKernelDelayThread(200000);
    int batteryLifeTime = 0;
    clearScreen(0);
    printf("Power:\n");
    printf("CIRCLE: go back to the main menu.\n");
    printf("RIGHT TRIGGER: reboot.\n");
    printf("External power: %s\n", scePowerIsPowerOnline() ? "yes" : "no ");
    printf("Low charge: %s\n", scePowerIsLowBattery() ? "yes" : "no ");
    printf("Charging: %s\n", scePowerIsBatteryCharging() ? "yes" : "no ");
    batteryLifeTime = scePowerGetBatteryLifeTime();
    printf("Battery life time: (%02dh%02dm)\n", batteryLifeTime / 60, batteryLifeTime - (batteryLifeTime / 60 * 60));
    while (1)
    {

        switch (get_key(0))
        {

        case SCE_CTRL_CIRCLE:
            main();
            break;
        case SCE_CTRL_RTRIGGER:
            scePowerRequestColdReset();
            break;
        default:
            printf("Invalid input\n");
            sceKernelDelayThread(1000000);
            powerPage();
            break;
        }
    }
}

int main()
{
    psvDebugScreenInit();
    clearScreen(0);
    printf("Vita Manager by inthecatsdreams\n");
    printf("LEFT TRIGGER: Manage volume\n");
    printf("RIGHT TRIGGER: Manage database\n");
    printf("CIRCLE: Manage taihen\n");
    printf("TRIANGLE: Battery stats\n");
    sceKernelDelayThread(1000000);
    while (1)
    {
        switch (get_key(0))
        {
        case SCE_CTRL_RTRIGGER:
            databasePage();
            break;
        case SCE_CTRL_LTRIGGER:
            volumePage();
            break;
        case SCE_CTRL_TRIANGLE:
            powerPage();
            break;
        case SCE_CTRL_CIRCLE:
            taiHenPage();
            break;
        default:
            printf("Wrong input\n");
            sceKernelDelayThread(1000000);
            main();
            break;
        }
    }
    return 0;
}