#include<stdlib.c>
#include<stdint.c>

#define BYTES_PER_PIXEL 2

uint16_t bitmap_draw(   unsigned int width, unsigned int height,
                        unsigned int bytes_per_pixel,
                        unsigned char *pixel_data){
    
    if(bytes_per_pixel != BYTES_PER_PIXEL){
        return -1
    }

    SSD1963_SetArea(0, 0, width - 1, height - 1);
    SSD1963_WriteCommand(CMD_WR_MEMSTART);

    for(int i = 0; i < width * height; i++){
        SSD193_WriteData(((uint16_t*)pixel_data)[i]);
    }

    return 0;
}
