#ifndef DRAW_H
#define DRAW_H

/**
 * @brief       Draw bitmap
 *
 * @param       width               Bitmap width
 * @param       height              Bitmap height
 * @param       bytes_per_pixel     Bytes per pixel
 * @param       pixel_data         uint16_t Bitmap data array 
 * @return      -1 if error 0 else
 *
 */
int8_t bitmap_draw(unsigned int width, unsigned int height, unsigned int bytes_per_pixel, const unsigned char *pixel_data);

#endif /* DRAW_H */
