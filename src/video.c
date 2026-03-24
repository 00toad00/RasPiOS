#include <video.h>

void printcp(char c, uint32_t color, uint32_t px, uint32_t py, Framebuffer *fb) {
    if (c < 32 || c > 126) { return; }
    const uint8_t *glyph = ascii_font[c-32];
    for (uint8_t y = 0; y < 13; y++) {
        uint8_t row = glyph[12-y];
        for (uint8_t x = 0; x < 8; x++) {
            if (row & (1 << (7 - x))) {
                uint8_t *pixel = (uint8_t*)fb->lfb + (py+y)*fb->pitch + 4*(px+x);
                *(uint32_t*)pixel = color;
            }
        }
    }
}

uint32_t printsp(const string str, uint32_t color, uint32_t px, uint32_t py, Framebuffer *fb) {
    uint16_t len = strlen16(str);
    for (uint16_t i = 0; i < len; i++) {
        printcp(str[i], color, px, py, fb);
        px += 10;
    }
    return px;
}

void println(const string str, Framebuffer *fb) {
    uint16_t len = strlen16(str);
    for (uint16_t i = 0; i < len; i++) {
        if (str[i] == '\n') {
            fb->xoff = 0;
            fb->yoff += 15;
            continue;
        }
        else if (fb->xoff+10 > fb->ex) {
            fb->xoff = 0;
            fb->yoff += 15;
        }
        printcp(str[i], fb->forecolor, fb->sx+fb->xoff, fb->sy+fb->yoff, fb);
        fb->xoff += 10;
    }
}

void printfs(const string str, const string istr, Framebuffer *fb) {
    uint16_t len = strlen16(str);
    for (uint16_t i = 0; i < len; i++) {
        if (str[i] == '\n') {
            fb->xoff = 0;
            fb->yoff += 15;
            continue;
        }
        else if (fb->xoff+10 > fb->ex) {
            fb->xoff = 0;
            fb->yoff += 15;
        }
        if (str[i] == '/') {
            if (str[i+1] == 's') { 
                println(istr, fb); 
                i++;
                continue;
            }
            else if (str[i+1] == 0) { return; }
        }
        printcp(str[i], fb->forecolor, fb->sx+fb->xoff, fb->sy+fb->yoff, fb);
        fb->xoff += 10;  
    }
}

void printfi(const string str, uint64_t ivalue, Framebuffer *fb) {
    uint16_t len = strlen16(str);
    ustring istr[66];
    for (uint16_t i = 0; i < len; i++) {
        if (str[i] == '\n') {
            fb->xoff = 0;
            fb->yoff += 15;
            continue;
        }
        else if (fb->xoff+10 > fb->ex) {
            fb->xoff = 0;
            fb->yoff += 15;
        }
        if (str[i] == '/') {
            switch (str[i+1]) {
                case 0: return;
                case 'h':
                    itoa_hex(ivalue, istr);
                    break;
                case 'b':
                    itoa_bin(ivalue, istr);
                    break;
                default:
                    itoa_int(ivalue, istr);
                    break;
            }
            println(istr, fb);
            i++;
            continue;
        }
        printcp(str[i], fb->forecolor, fb->sx+fb->xoff, fb->sy+fb->yoff, fb);
        fb->xoff += 10;         
    }
}

void clear_screen(Framebuffer *fb) {
    for (uint32_t y = 0; y < fb->height; y++) {
        for (uint32_t x = 0; x < fb->width; x++) {
            uint8_t *pixel = (uint8_t*)(fb->lfb + y*fb->pitch + 4*x);
            *(uint32_t*)pixel = fb->backcolor;
        }
    }
}

void draw_line(uint32_t color, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, Framebuffer *fb) {
    if (y0 == y1) {
        while (x0 <= x1) {
            uint8_t *pixel = (uint8_t*)fb->lfb + y0*fb->pitch + 4*x0;
            *(uint32_t*)pixel = color;
            x0++;
        }
    } else if (x0 == x1) {
        while (y0 <= y1) {
            uint8_t *pixel = (uint8_t*)fb->lfb + y0*fb->pitch + 4*x0;
            *(uint32_t*)pixel = color;
            y0++; 
        }       
    } else {
        int32_t dx = (int32_t)x1 - (int32_t)x0;
        int32_t dy = (int32_t)y1 - (int32_t)y0;

        int32_t abs_dx = dx;
        if (abs_dx < 0) abs_dx = -abs_dx;

        int32_t abs_dy = dy;
        if (abs_dy < 0) abs_dy = -abs_dy;

        int32_t sx = 1;
        if (dx < 0) sx = -1;

        int32_t sy = 1;
        if (dy < 0) sy = -1;

        int32_t err;
        if (abs_dx > abs_dy) err = abs_dx / 2;
        else err = -abs_dy / 2;

        int32_t e2;
        int32_t x = (int32_t)x0;
        int32_t y = (int32_t)y0;

        while (1) {
            uint8_t *pixel = (uint8_t*)fb->lfb + y * fb->pitch + 4 * x;
            *(uint32_t*)pixel = color;

            if (x == (int32_t)x1 && y == (int32_t)y1) break;

            e2 = err;
            if (e2 > -abs_dx) { 
                err -= abs_dy; 
                x += sx; 
            }
            if (e2 < abs_dy) { 
                err += abs_dx; 
                y += sy; 
            }
        }
    }
}

void nextrow(Framebuffer *fb) { 
    fb->xoff = 0;
    fb->yoff += 15;
}

uint32_t argb(uint32_t argb, PixelOrder porder) {
    if (porder == ARGB) { return argb; }
    uint32_t abgr;
    uint32_t a = argb & 0xFF000000;
    uint32_t r = argb & 0x00FF0000;
    uint32_t g = argb & 0x0000FF00;
    uint32_t b = argb & 0x000000FF;

    abgr = a | (b << 16) | g | (r >> 16);
    return abgr;
}