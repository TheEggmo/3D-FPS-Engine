#ifndef TOOLS_H
#define TOOLS_H

#include <QImage>
#include <math.h>
#include <fstream>
#include <strstream>
#include <ctype.h>



class Tools{
public:
    class Color;
    class Color8;
    class Vector2;
//    class Vector3;
//    class Mat4x4;
//    class Triangle;
//    class Mesh;

    // Custom color classes. Color stores colors in floats of range 0.0 to 1.0. Color8 stores colors in ints of range 0 to 255
    class Color{
    public:
        float r, g, b;
        Color(int r, int g, int b){
            this->r = r;
            this->g = g;
            this->b = b;
        };
        Color(){
            this->r = 1;
            this->g = 1;
            this->b = 1;
        };
        // Converts to Color8
        Color8 convert(){
            return Color8(r*255, g*255, b*255);
        }
    };
    class Color8{
    public:
        int r, g, b;
        Color8(int r, int g, int b){
            this->r = r;
            this->g = g;
            this->b = b;
        };
        Color8(){
            this->r = 255;
            this->g = 255;
            this->b = 255;
        };
        Color8 operator*(float k){
            Color8 out;
            out.r *= k;
            out.g *= k;
            out.b *= k;
            return out;
        }
        // Converts to Color
        Color convert(){
            return Color(float(r)/255, float(g)/255, float(b)/255);
        }
    };

    // Class repsresenting a 2d vector
    class Vector2{
    public:
        int x, y;
        Vector2(int x, int y){
            this->x = x;
            this->y = y;
        }
        Vector2(){
            this->x = 0;
            this->y = 0;
        }
        float distanceToPoint(Vector2 p){
            return sqrt(pow(this->x - p.x, 2) + pow(this->y - p.y, 2));
        }
        Vector2 operator+(Vector2 v){
            return Vector2(this->x + v.x, this->y + v.y);
        }
        Vector2 operator-(Vector2 v){
            return Vector2(this->x - v.x, this->y - v.y);
        }
    };



    // Commonly used functions for drawing

    // Draws on the specified image, at the specified x,y coordinates, using the specified color
    static void drawPixel(QImage *image, int x, int y, Color8 color);
    static void drawPixel(QImage *image, int x, int y, Color color);
    // Draws a line on the specified image from the coordinates x0,y0 to x1,y1, using the specified color
    static void drawLine(QImage *image, int x0, int y0, int x1, int y1, Color8 color);
    static void drawLine(QImage *image, int x0, int y0, int x1, int y1, Color color);


    // Math functions

    // Distance between two points in 2d space
    static float distanceToPoint(int x0, int y0, int x1, int y1);



};
#endif // TOOLS_H
