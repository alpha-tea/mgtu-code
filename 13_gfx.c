#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Рисование или отдельных схем или условного видеоряда в графический файл.
// Система коодинаты начинается с левого верхнего угла.

#define DATA_MAX 0x100
#define TEXT_MAX 0x100
#define SCR_MAX_X 0x1000
#define SCR_MAX_Y 0x1000

struct pixel {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

// Константы.
static const int bmp_header_size = 0x36;
static const int bmp_width = 0x12;
static const int bmp_height = 0x16;
static const int bmp_planes = 0x1A;
static const int bmp_bpp = 0x1C;
static const int pixel_comp = 3;
static const int true_color_bpp = 0x18;
static const int word_high = 0xFF00;
static const int word_low = 0x00FF;
static const int vscr_cx = 0x0280;
static const int vscr_cy = 0x0140;
static const int vscr_cz = 0x0100;

// Глобальные переменные и данные для всей программы.
static unsigned char bmp_header[] = {
    0x42, 0x4D, 0x36, 0x80, 0x25, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x80, 0x02,
    0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char data[DATA_MAX];
static int image_offset = 0x36, image_size = 0;
static struct pixel screen[SCR_MAX_Y * SCR_MAX_X];
static int scr_sx = 0, scr_sy = 0, scr_sz = 0x1000;
static char txt[TEXT_MAX];

void save_to_file(const char name[])
{   // Сохранение буффера экрана в файл .BMP. Без проверки имени файла, но обработкой ошибки, файл будет перезаписан.
    FILE* bmp_file = fopen(name, "wb");
    if (bmp_file == NULL || strlen(name) == 0 || scr_sx == 0 || scr_sy == 0) {
        printf("File name is empty, error creating file or sizes are incorrect.\n");
        return;
    }
    fwrite(bmp_header, bmp_header_size, 1, bmp_file);
    fwrite(screen, sizeof(struct pixel), scr_sx * scr_sy, bmp_file);
    fclose(bmp_file);
}

void load_from_file(const char name[])
{   //
    /*    // FILE* bmp_file = fopen("scr_0000.bmp", "rb");
    for (int i = 0; i < 19; ++i) {
        fread(data, sizeof(unsigned char), 1, bmp_file);
        printf("%x ", data[0]);
    }
    fseek(bmp_file, 54, SEEK_SET);
    printf("\n");
    for (int i = 0; i < 3; ++i) {
        fread(data, sizeof(unsigned char), 1, bmp_file);
        printf("%x ", data[0]);
    }
    fclose(bmp_file);
*/
}

/*
void clear_screen(struct pixel* color, int is_debug)
{   // Очистка или заполнение буффера заданным цветом и вывод отладочной таблицы если требуется.
    if (!is_debug) {
        printf("Clear screen with one color (%hhu, %hhu, %hhu).\n", color->red, color->green, color->blue);
        for (int i = 0; i < scr_sy; ++i)
            for (int j = 0; j < scr_sx; ++j)
                screen[i * scr_sx + j] = *color;                    // Или копировать по компонентам.
    } else {
        int range = 0, range_max = scr_sx / (UCHAR_MAX + 1), range_rem = scr_sx % (UCHAR_MAX + 1);
        int range_all = range_max * (UCHAR_MAX + 1);
        printf("Clear screen and fill possible area of color components.\n");
        struct pixel pix = { .red = 0, .green = 0, .blue = 0 }, empty = { .red = 0, .green = 0, .blue = 0 };
        for (int i = 0; i < scr_sy; i++) {
            for (int j = 0; j < range_all; ++j) {
                screen[i * scr_sx + j] = pix;
                if (j % (UCHAR_MAX + 1) == 0)
                    range = (range + 1) % range_max;
                if (range < pixel_comp) {
                    ((unsigned char*)&pix)[range]++;
                } else {
                    pix.red++; pix.green++; pix.blue++;
                }
            }
            for (int j = 0; j < range_rem; ++j)
                screen[i * scr_sx + range_all + j] = empty;
        }
    }
}
*/
void get_pixel(struct pixel* pix, int px, int py)
{   // Получение пикселя по заданным координатам и сохранение его в объект назначения по адресу.
    if (pix == NULL || px < 0 || px > scr_sx || py < 0 || py > scr_sy) {
        printf("Error get pixel, structure address (%p) is null or position [%d,%d] incorrect.\n", pix, px, py);
        return;
    }
    *pix = screen[py * scr_sx + px];
}

void set_pixel(struct pixel* pix, int px, int py)
{   // Установка точки в буффере по заданному цвету и координатам.
    if (pix == NULL || px < 0 || px > scr_sx || py < 0 || py > scr_sy) {
        printf("Error set pixel, structure address (%p) is null or position [%d,%d] incorrect.\n", pix, px, py);
        return;
    }
    screen[(scr_sy - 1 - py) * scr_sx + px] = *pix;
}

void set_pixel_3D(struct pixel* pix, int px, int py, int pz)
{   // Камера условно в центре экрана.
    if (pix == NULL || px < 0 || px > scr_sx || py < 0 || py > scr_sy || pz <= 0 || pz > scr_sz) {
        printf("Error set pixel 3D, structure address (%p) is null or position [%d, %d, %d] incorrect.\n",
               pix, px, py, pz);
        return;
    }
    int cy = scr_sy >> 0x01, cx = scr_sx >> 0x01;
    //printf("Set 3D at: (%d, %d, %d), virtual center (%d, %d, %d), center screen (%d, %d) ",
    //       px, py, pz, vscr_cx, vscr_cy, vscr_cz, cx, cy);
    px = (px - vscr_cx) / pz + cx; py = (py - vscr_cy) / pz + cy;
    // px = (px) / pz + cx; py = (py) / pz + cy;
    //printf("pixel 2D coords: (%d, %d).\n", px, py);
    set_pixel(pix, px, py);
}

void shift_pixel(struct pixel* pix, int step)
{   // Увеличивает/уменьшает все компоненты на шаг, но только до максимума в 255, если превышает то пропускаем.
    if ((step > 0 && pix->red + step <= UCHAR_MAX) || (step < 0 && pix->red + step >= 0))
        pix->red += step;
    if ((step > 0 && pix->green + step <= UCHAR_MAX) || (step < 0 && pix->green + step >= 0))
        pix->green += step;
    if ((step > 0 && pix->blue + step <= UCHAR_MAX) || (step < 0 && pix->blue + step >= 0))
        pix->blue += step;
}

void draw_sinus(int size_y, int is_grad, double shift)
{   // Рисование синуса по всей ширине высоте экрана с флагом градиента.
    const double pi = acos(-1.0);
    int scr_center_y = (scr_sy >> 0x01) - 1, grad_color = 0;
    struct pixel pix = { .red = 0x00, .green = 0x00, .blue = 0x00 };
    for (int i = 0; i < scr_sx; ++i) {
        double x = (double)i / (double)(scr_sx) * (2.0 * pi) + shift;
        double y = (scr_center_y + round(sin(x) * (double)size_y));
        // printf("%.2f, %.2f.\n", x, y);
        set_pixel(&pix, i, (int)y);
        if (is_grad) {
            if ((++((char*)&pix)[grad_color]) == 0)
                grad_color = (grad_color + 1) % pixel_comp;
            struct pixel grad_down = pix, grad_up = pix;
            unsigned char color_d = ((unsigned char*)&pix)[grad_color];
            unsigned char color_u = ((unsigned char*)&pix)[grad_color];
            int y_up = (int)y, y_down = (int)y;
            while (color_d++ < 0xFA && color_u++ < 0xFA && (y_down > 0 && y_down < scr_sy) &&
                   (y_up > 0 && y_up < scr_sy)) {
                set_pixel(&grad_down, i, --y_down);
                set_pixel(&grad_up, i, ++y_up);
                ((char*)&grad_down)[grad_color] += 2;
                color_d = ((char*)&grad_down)[grad_color];
                ((char*)&grad_up)[grad_color] += 2;
                color_u = ((char*)&grad_down)[grad_color];
            }
        }
    }
}

void draw_circle(int radius, int step)
{   // Рисование плоской окружности с заполнением точками через шаг.
    int cx = scr_sx / 2, cy = scr_sy / 2;
    for (int i = -radius, j; i < radius; i += step) {
        for (j = -radius; j < radius; j += step) {
            double x = j, y = i, r = radius;
            double z = pow(x, 2.0) + pow(y, 2.0);
            struct pixel pix = { .red = 0x00, .green = 0x00, .blue = 0xFF };
            if (pow(r, 2.0) >= z)
                set_pixel(&pix, cx + i, cy + j);
        }
    }
}

void draw_sphere(int radius, int step)
{   // Рисование трехмерного объекта, в центре виртуального экрана.
    printf("Draw sphere with radius %d and step %d.\n", radius, step);
    // vscr_cy - radius / 2, vscr_cx - radius / 2.
    radius = 256; step = 1;
    int cx = scr_sx / 2, cy = scr_sy / 2;
    for (int i = -radius * 3, j; i < radius * 3; i += step) {
        for (j = -radius * 3; j < radius * 3; j += step) {
            double x = i / 3.0, y = j / 3.0, r = radius;
            double z = pow(r, 2.0) - pow(x, 2.0) - pow(y, 2.0);
            if (z > 0) {
                z = sqrt(z);
                int k = (int)round(z / 2.0);
                // printf("%.2f %.2f %.2f - %d:%d:%d\n", x, y, z, j, i, k);
                struct pixel pix = { .red = 0x00, .green = 0x00, .blue = 0xFF };
                set_pixel_3D(&pix, cx + (int)x, cy + (int)y, k);
                // set_pixel(&pix, scr_sx / 2 + i, scr_sy / 2 + j);
            } else {
                // printf("%.2f %.2f - not exist.\n", x, y);
            }
        }
    }
}

void gfx(const char file_name[], int size_x, int size_y)
{
    printf("Create GFX to bmp file(s) with size %dx%d and 24-bit color.\n\n", size_x, size_y);
    if (size_x <= 0 || size_x > SCR_MAX_X || size_y <= 0 || size_y > SCR_MAX_Y || strlen(file_name) == 0) {
        printf("Size of screen buffer horizontal %d, vertical %d or file name is empty.\n", size_x, size_y);
        return;
    }
    scr_sx = size_x; scr_sy = size_y;
    printf("Set parameters of .BMP header to file '%s', size of header %d:\n", file_name, bmp_header_size);
    bmp_header[bmp_width] = scr_sx & word_low; bmp_header[bmp_width + 1] = scr_sx >> CHAR_BIT;
    bmp_header[bmp_height] = scr_sy & word_low; bmp_header[bmp_height + 1] = scr_sy >> CHAR_BIT;
    printf("Set width of image to %hu, height to %hu;\n",
           ((unsigned short*)bmp_header)[bmp_width >> 0x01], ((unsigned short*)bmp_header)[bmp_height >> 0x01]);
    bmp_header[bmp_planes] = 0x01; bmp_header[bmp_bpp] = true_color_bpp;
    printf("Set planes to %hu(default) and color depth to %hu bits per pixel.\n",
           ((unsigned short*)bmp_header)[bmp_planes >> 0x01], ((unsigned short*)bmp_header)[bmp_bpp >> 0x01]);
    image_size = scr_sx * scr_sy * (true_color_bpp / CHAR_BIT);
    printf("\nImage offset for pixels in file %+d and size of image %d bytes.\n", image_offset, image_size);
    printf("Virtual screen max %u horizontal and %u vertical, size is %u bytes.\n", SCR_MAX_X, SCR_MAX_Y, sizeof(screen));
    printf("Data offsets and sizes of structure 'pixel':\nOffset:\t\tSize:\tComment:\n");
    struct pixel pix = { .red = 255, .green = 255, .blue = 255 };
    printf("%u\t\t%u\tred color component of pixel;\n", (void*)&pix.red - (void*)&pix, sizeof(pix.red));
    printf("%u\t\t%u\tgreen color component of pixel;\n", (void*)&pix.green - (void*)&pix, sizeof(pix.green));
    printf("%u\t\t%u\tblue color component of pixel.\n\n", (void*)&pix.blue - (void*)&pix, sizeof(pix.blue));
    clear_screen(&pix, 0);
    printf("Trying to use main functions and draw some figures.\n");
    pix.red = 0; pix.blue = 0; pix.green = 255;
    set_pixel(&pix, scr_sx >> 0x01, scr_sy >> 0x01);
    get_pixel(&pix, scr_sx >> 0x01, scr_sy >> 0x01);
    // save_to_file("scr_dst.bmp");
    printf("Get pixel from position [%d,%d], color (%d, %d, %d).\n", 257, 1, pix.red, pix.green, pix.blue);
/*
    pix.green = pix.blue = pix. red = 0xFF;
    char file[] = "scr_000.bmp";
    for (int i = 0; i < 30; ++i) {
        clear_screen(&pix, 0);
        file[4] = i / 100 + '0';
        file[5] = i / 10 % 10 + '0';
        file[6] = i % 10 + '0';
        draw_sinus(0x80, 1, 0.5 * i);
        save_to_file(file);
    }
*/
    pix.green = pix.blue = pix. red = 0xFF;
    clear_screen(&pix, 0);
    pix.green = pix.blue = pix. red = 0x00;
    // draw_circle(256, 2);

    set_pixel_3D(&pix, 0, 0, 1);
    set_pixel_3D(&pix, 1279, 639, 1);
    set_pixel_3D(&pix, 1279, 639, 64);  // Приближаемся к центру.

    draw_sphere(24, 1);
    save_to_file("scr_3d.bmp");
}




