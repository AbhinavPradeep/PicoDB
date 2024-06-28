#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/flash.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"

#define BUTTON 15
#define SECTOR_SIZE 4096

extern char __flash_binary_end;

void getLine(char* buffer);
void saveInMemory(char* string);
void processInput(char* buffer);
char* retreiveFromMemory();

int main() {
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed");
        return -1;
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    uintptr_t end = (uintptr_t) &__flash_binary_end;
    gpio_init(BUTTON);
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_pull_up(BUTTON);
    while(gpio_get(BUTTON)) {
        ;
    }
    printf("Ends at 0x%08x \n\n",end);
    while(1) {
	sleep_ms(10);
        printf("Enter input: ");
	char buffer[SECTOR_SIZE] = {0};
	getLine(buffer);
	printf("Buffer created at: 0x%08x \n", &buffer);
	processInput(buffer);
	sleep_ms(10);
	printf("\n");
    }
}

void processInput(char* buffer) {
    sleep_ms(10);
    printf("You input: %s\n",buffer);	
    if(strcmp(buffer,"print") == 0) {
        char* retreived = retreiveFromMemory();
	sleep_ms(10);
	printf("Retrieved: %s\n",retreived);
	fflush(stdout);
    } else {
	sleep_ms(10);
	printf("Saving in memory...\n");
        saveInMemory(buffer);
    }
}

void getLine(char* buffer) {
    int i = 0;
    while(1) {
        char c = getchar();
	if(c != '\n' && c != '\r') {
	    buffer[i++] = c;    
	} else {
	    break;
	}
    }
    buffer[i] = '\0';
}

void saveInMemory(char* string) {
    //Convert __flash_binary_end to integer pointer
    uintptr_t flashEndAddress = (uintptr_t)&__flash_binary_end;
    //Minimum addressable block is SECTOR_SIZE bytes wide. 
    //To find the next suitable block, add SECTOR_SIZE to the end of
    //the binary and round down to the nearest block boundry
    uintptr_t nearestBlock = (flashEndAddress + SECTOR_SIZE) & 0xfffff000;
    //All hardware/fash functions work on offset.
    uint32_t offSet = nearestBlock - XIP_BASE;
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(offSet,SECTOR_SIZE);
    flash_range_program(offSet, (const uint8_t*)string, SECTOR_SIZE);
    restore_interrupts (ints);
}

char* retreiveFromMemory() {
    uintptr_t flashEndAddress = (uintptr_t)&__flash_binary_end;
    uintptr_t nearestBlock = (flashEndAddress + SECTOR_SIZE) & 0xfffff000;
    sleep_ms(10);
    printf("Sector starts at: 0x%08x\n", nearestBlock);
    uint32_t ints = save_and_disable_interrupts();
    char* memoryLocation = (char*) nearestBlock;
    //Static to be able to return it
    static char staticBuffer[SECTOR_SIZE];
    printf("Static buffer created at: 0x%08x\n", &staticBuffer);
    memcpy(staticBuffer, memoryLocation, SECTOR_SIZE);
    restore_interrupts (ints);
    return staticBuffer;
}
