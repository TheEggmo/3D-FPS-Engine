#include "Tools.h"


Tools::Color8 Tools::getPixel(QImage *image, int x, int y){
    // If accessing outside of image bounds, return an invalid color
    if(x < 0 || y < 0 || x >= image->width() || y>= image->height()){
//        qDebug("ACCESSING OUT OF BOUNDS PIXELS, x: %d, y: %d", x, y);
        return Color8(-1, -1, -1);
    }

    Color8 out;

    uchar *pix = image->scanLine(y);
    int adr = 4 * x;
    out.b = pix[adr];
    out.g = pix[adr+1];
    out.r = pix[adr+2];

//    qDebug("%d %d %d", out.r, out.g, out.b);

    return out;
}

void Tools::drawPixel(QImage *image, int x, int y, Color8 color){
    // Prevent drawing outside of image bounds
    if(x < 0 || y < 0 || x >= image->width() || y>= image->height()){
            return;
    }

    uchar *pix = image->scanLine(y);
    int adr = 4 * x;
    pix[adr] = color.b;
    pix[adr+1] = color.g;
    pix[adr+2] = color.r;
    pix[adr+3] = 255;
}
void Tools::drawPixel(QImage *image, int x, int y, Color color){
    // Convert to Color8 and call again
    drawPixel(image, x, y, color.convert());
}

// Bresenham's line algorithm using pseudocode from wikipedia
void drawLineLow(QImage *image, int x0, int y0, int x1, int y1, Tools::Color8 color){
    float dx = x1 - x0;
    float dy = y1 - y0;
    float yi = 1;
    if(dy < 0){
        yi = -1;
        dy = -dy;
    }
    float D = 2 * dy - dx;
    float y = y0;
    for(float x = x0; x <= x1; x++){
        Tools::drawPixel(image, x, y, color);
        if(D > 0){
            y = y + yi;
            D = D - 2 * dx;
        }
        D = D + 2 * dy;
    }
}
void drawLineHigh(QImage *image, int x0, int y0, int x1, int y1, Tools::Color8 color){
    float dx = x1 - x0;
    float dy = y1 - y0;
    float xi = 1;
    if(dx < 0){
        xi = -1;
        dx = -dx;
    }
    float D = 2 * dx - dy;
    float x = x0;
    for(float y = y0; y <= y1; y++){
        Tools::drawPixel(image, x, y, color);
        if(D > 0){
            x = x + xi;
            D = D - 2 * dy;
        }
        D = D + 2 * dx;
    }
}
void Tools::drawLine(QImage *image, int x0, int y0, int x1, int y1, Color8 color){
    if(abs(y1 - y0) < abs(x1 - x0)){
        if(x0 > x1){
            drawLineLow(image, x1, y1, x0 ,y0, color);
        }else{
            drawLineLow(image, x0, y0, x1, y1, color);
        }
    }else{
        if(y0 > y1){
            drawLineHigh(image, x1, y1, x0, y0, color);
        }else{
            drawLineHigh(image, x0, y0, x1, y1, color);
        }
    }
}
void Tools::drawLine(QImage *image, int x0, int y0, int x1, int y1, Color color){
    // Convert to Color8 and call again
    drawLine(image, x0, y0, x1, y1, color.convert());
}


float Tools::distanceToPoint(int x0, int y0, int x1, int y1){
    return sqrt(pow(x0 - x1, 2) + pow(y0 - y1, 2));
}
