#include<stdint.h>
#include"ssd1963.h"
#include"ssd1963_lld.h"
#include"ssd1963_cmd.h"
#include"lcd_conf.h"
#include"draw.h"

#define BYTES_PER_PIXEL 2

int8_t bitmap_draw(    unsigned int width, unsigned int height,
                        unsigned int bytes_per_pixel,
                        const unsigned char *pixel_data){
    
    if(bytes_per_pixel != BYTES_PER_PIXEL){
        return -1;
    }

    SSD1963_SetArea(0, 0, width - 1, height - 1);
    SSD1963_WriteCommand(CMD_WR_MEMSTART);

    for(int i = 0; i < width * height; i++){
        SSD1963_WriteData(((uint16_t*)pixel_data)[i]);
    }

    return 0;
}
