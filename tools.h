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
        }
        Color(){
            this->r = 1;
            this->g = 1;
            this->b = 1;
        }
        // Converts to Color8
        Color8 convert(){
            return Color8(r*255, g*255, b*255);
        }
        // Check if the color is valid(it's rgb values are in range 0.0f-1.0f
        bool valid(){
            if(r > 1 || g > 1 || b > 1 || r < 0 || g < 0 || b < 0) return false;
            return true;
        }
    };
    class Color8{
    public:
        int r, g, b;
        Color8(int r, int g, int b){
            this->r = r;
            this->g = g;
            this->b = b;
        }
        Color8(){
            this->r = 255;
            this->g = 255;
            this->b = 255;
        }
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
        // Check if the color is valid(it's rgb values are in range 0-255
        bool valid(){
            if(r > 255 || g > 255 || b > 255 || r < 0 || g < 0 || b < 0) return false;
            return true;
        }

    };

    // Class repsresenting a 2d vector
    class Vector2{
    public:
        float x, y;

        Vector2(int x, int y){
            this->x = (float)x;
            this->y = (float)y;
        }
        Vector2(float x, float y){
            this->x = x;
            this->y = y;
        }
        Vector2(){
            this->x = 0;
            this->y = 0;
        }
        Vector2 operator+(Vector2 v){
            return Vector2(this->x + v.x, this->y + v.y);
        }
        Vector2 operator-(Vector2 v){
            return Vector2(this->x - v.x, this->y - v.y);
        }
        Vector2 operator*(float k){
            return Vector2(this->x * k, this->y * k);
        }
        Vector2 operator/(float k){
            return Vector2(this->x / k, this->y / k);
        }

        float length(){
            return sqrt(x*x + y*y);
        }
        float distanceToPoint(Vector2 p){
            return sqrt(pow(this->x - p.x, 2) + pow(this->y - p.y, 2));
        }
        Vector2 normalized(){
            return *this/this->length();
        }
    };

    // Class representing a 2D rectangle
    class Rect{
    public:
        // The rectangle is defined by two points
        Vector2 topLeft, botRight;
        // Rotation in radians (UNUSED)
        // The functions in this class can be remade to account for rotations,
        // but I don't think it's necessary
        float rotation = 0.0f;

        Rect(){
            topLeft = {0,0};
            botRight = {0,0};
        }
        Rect(Vector2 a, Vector2 b){
            topLeft = a;
            botRight = b;
        }

        // Swaps the vectors if they are "wrong"
        // I.e. topLeft isn't actually the top left point
        void fix(){
            if(topLeft.x > botRight.x || topLeft.y > botRight.y){
                Vector2 t = topLeft;
                this->topLeft = botRight;
                this->botRight = t;
            }
        }
        // Check if this rectangle is colliding with the specified rectangle
        bool collides(Rect other){
//            if(this->botRight.y < other.topLeft.y || this->topLeft.x < other.botRight.x) return false;
//            return true;
            return !(this->botRight.y < other.topLeft.y || this->topLeft.x < other.botRight.x);
        }
    };

    // Commonly used functions for drawing

    // Extracts the color value of the specified pixel
    static Color8 getPixel(QImage *image, int x, int y);
    // Draws on the specified image, at the specified x,y coordinates, using the specified color
    static void drawPixel(QImage *image, int x, int y, Color8 color);
    static void drawPixel(QImage *image, int x, int y, Color color);
    // Draws a line on the specified image from the coordinates x0,y0 to x1,y1, using the specified color
    static void drawLine(QImage *image, int x0, int y0, int x1, int y1, Color8 color);
    static void drawLine(QImage *image, int x0, int y0, int x1, int y1, Color color);


    // Math functions

    // Distance between two points in 2d space
    static float distanceToPoint(int x0, int y0, int x1, int y1);

    // Return value 'in' clamped between 'lo' and 'hi'
    static float clamp(float in, float lo, float hi){
        if(in < lo) return lo;
        if(in > hi) return hi;
        return in;
    }
    // Linearly interpolate value 'from' to 'to' by 'mod'
    static float lerp(float from, float to, float mod){
        return from + (to - from) * mod;
    }

};
#endif // TOOLS_H
