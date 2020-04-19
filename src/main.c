#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vitasdk.h>

#include "ctrl.h"
#include "debugScreen.h"

#define printf psvDebugScreenPrintf
#define clearScreen psvDebugScreenClear



void increaseVolume(int vol)
{
  clearScreen();
  if (vol == 30){
      printf("You are alredy at the maxium. Nothing to do.");
      volumePage();
    }
  else
    {
      int ret = sceRegMgrSetKeyInt ("/CONFIG/SOUND/", "main_volume", vol + 1);
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
  int ret = sceRegMgrSetKeyInt ("/CONFIG/SOUND/", "main_volume", 0);
  volumePage();
}

void databasePage()
{
  
  clearScreen(0);
  sceKernelDelayThread(200000);
  printf("Press R trigger to rebuild database\n");
  printf("Press CIRCLE to go back to the main menu\n");
  while(1){
      switch(get_key(0)){
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
          printf("Invalid input");
          main();
      }
	    
      
	}

  }



void decreaseVolume (int vol){
  clearScreen();
  if (vol == 0){
      volumePage();
    }
  else{
      int ret = sceRegMgrSetKeyInt ("/CONFIG/SOUND/", "main_volume", vol - 1);
      volumePage();
    }
}

void volumePage(){
  clearScreen(0);
  printf("Volume:\n");
  printf("ARROW UP: increase volume.\n");
  printf("ARROW DOWN: decrease volume.\n");
  printf("TRIANGLE: mute console\n");
  printf("SQUARE: apply changes and reboot\n");
  printf("CIRCLE: go back to main menu\n");
  int currentVolume = getCurrentVolume();
  printf("Current volume %d", currentVolume);
  while (1){

  switch (get_key (0)){
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
	  break;
	
}
}
}

void powerPage(){
  sceKernelDelayThread(200000);
  int batteryLifeTime = 0;
  //credits to the vitasdk samples for this one.
  clearScreen(0);
  printf("Power:\n");
  printf("CIRCLE: go back to the main menu.\n");
  printf("RIGHT TRIGGER: reboot.\n");
  printf("External power: %s\n", scePowerIsPowerOnline()? "yes" : "no ");
	printf("Low charge: %s\n", scePowerIsLowBattery()? "yes" : "no ");
	printf("Charging: %s\n", scePowerIsBatteryCharging()? "yes" : "no ");
  batteryLifeTime = scePowerGetBatteryLifeTime();
	printf("Battery life time: (%02dh%02dm)\n", batteryLifeTime/60, batteryLifeTime-(batteryLifeTime/60*60));
  while (1){

  switch (get_key (0)){
	  
	case SCE_CTRL_CIRCLE:
	  main();
	  break;
	case SCE_CTRL_RTRIGGER:
	  scePowerRequestColdReset();
	  break;
	default:
	  break;
	
  }

}
}

int main(){
  psvDebugScreenInit();
  clearScreen(0);
  printf("Vita Manager by inthecatsdreams\n");
  printf("LEFT TRIGGER: Manage volume\n");
  printf("RIGHT TRIGGER: Manage database\n");
  printf("TRIANGLE: Power page\n");
  sceKernelDelayThread(100000);
  while (1){
    switch(get_key (0)){
	    case SCE_CTRL_RTRIGGER:
	      databasePage();
	      break;
	    case SCE_CTRL_LTRIGGER:
	      volumePage();
	      break;
      case SCE_CTRL_TRIANGLE:
        powerPage();
        break;
	    default:
	      break;
	
  }
}
  return 0;
}
