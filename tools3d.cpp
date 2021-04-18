#include "tools3d.h"

using T2 = Tools;

/*--------------------------------------------------------------->*/
/*--------------------------------------------------------------->*/
/*--------------------------------------------------------------->*/
/*----------------------- Mesh functions ------------------------>*/
/*--------------------------------------------------------------->*/
/*--------------------------------------------------------------->*/
/*--------------------------------------------------------------->*/

bool Tools3D::Mesh::loadFromFile(const QString& fileName){
    tris.clear();
    QFile file(fileName);
    if(file.exists()){
        if(file.open(QFile::ReadOnly | QFile::Text)){
            std::vector<Vector3> v;//, vn;
//                std::vector<UV> vt;

            while(!file.atEnd()){
                // Trim and split the next line.
                // Splitting works based on a regex looking for whitespaces.
                QString line = file.readLine().trimmed();
                QStringList lineParts = line.split(QRegularExpression("\\s+"));
                // Process the line based on its first part
                if(lineParts.count() > 0){
                    if(lineParts.at(0).compare("#", Qt::CaseInsensitive) == 0){
                        //COMMENT
                    }else if(lineParts.at(0).compare("v", Qt::CaseInsensitive) == 0){
                        //VERTICES
                        v.push_back({lineParts.at(1).toFloat(),lineParts.at(2).toFloat(),lineParts.at(3).toFloat()});
                    }else if(lineParts.at(0).compare("vn", Qt::CaseInsensitive) == 0){
                        //NORMAL
                    }else if(lineParts.at(0).compare("vt", Qt::CaseInsensitive) == 0){
                        //TEXTURE
                    }else if(lineParts.at(0).compare("f", Qt::CaseInsensitive) == 0){
                        //FACE DATA
                        // Faces must be triangles
                        Tools3D::Triangle tri;
                        // Vertices
                        tri.p[0] = v.at(lineParts.at(1).split("/").at(0).toInt() - 1);
                        tri.p[1] = v.at(lineParts.at(2).split("/").at(0).toInt() - 1);
                        tri.p[2] = v.at(lineParts.at(3).split("/").at(0).toInt() - 1);
                        tris.push_back(tri);
                    }
                }
            }
        }else{
            return false;
        }
    }else{
        return false;
    }
    return true;
}
void Tools3D::Mesh::scale(float mod){
    for(int tIdx = 0; tIdx < tris.size(); tIdx++){
        for(int pIdx = 0; pIdx <= 2; pIdx++){
//            tris[tIdx].p[pIdx] = tris[tIdx].p[pIdx] * mod;
            tris[tIdx].p[pIdx] *= mod;
        }
    }
}
void Tools3D::Mesh::scaleX(float mod){
    for(int tIdx = 0; tIdx < tris.size(); tIdx++){
        for(int pIdx = 0; pIdx <= 2; pIdx++){
//            tris[tIdx].p[pIdx].x = tris[tIdx].p[pIdx].x * mod;
            tris[tIdx].p[pIdx].x *= mod;
        }
    }
}
void Tools3D::Mesh::scaleY(float mod){
    for(int tIdx = 0; tIdx < tris.size(); tIdx++){
        for(int pIdx = 0; pIdx <= 2; pIdx++){
//            tris[tIdx].p[pIdx].y = tris[tIdx].p[pIdx].y * mod;
            tris[tIdx].p[pIdx].y *= mod;
        }
    }
}
void Tools3D::Mesh::scaleZ(float mod){
    for(int tIdx = 0; tIdx < tris.size(); tIdx++){
        for(int pIdx = 0; pIdx <= 2; pIdx++){
//            tris[tIdx].p[pIdx].z = tris[tIdx].p[pIdx].z * mod;
            tris[tIdx].p[pIdx].z *= mod;
        }
    }
}
void Tools3D::Mesh::move(Tools3D::Vector3 v){
    Mat4x4 mat = newMatTrans(v.x, v.y, v.z);
    for(int i = 0; i < tris.size(); i++){
        tris[i] = tris[i] * mat;
//        tris[i].p[0] = tris[i].p[0] + v;
//        tris[i].p[1] = tris[i].p[1] + v;
//        tris[i].p[2] = tris[i].p[2] + v;
    }
}
bool Tools3D::Mesh::empty(){
    return tris.empty();
}

bool Tools3D::MeshTexture::loadFromFile(const QString& fileName){
    flat = false;
    tris.clear();
    QFile file(fileName);
    if(file.exists()){
        if(file.open(QFile::ReadOnly | QFile::Text)){
            std::vector<Vector3> v;//, vn;
            std::vector<UV> vt;

            while(!file.atEnd()){
                // Trim and split the next line.
                // Splitting works based on a regex looking for whitespaces.
                QString line = file.readLine().trimmed();
                QStringList lineParts = line.split(QRegularExpression("\\s+"));
                // Process the line based on its first part
                if(lineParts.count() > 0){
                    if(lineParts.at(0).compare("#", Qt::CaseInsensitive) == 0){
                        //COMMENT
                    }else if(lineParts.at(0).compare("v", Qt::CaseInsensitive) == 0){
                        //VERTICES
                        v.push_back({lineParts.at(1).toFloat(),lineParts.at(2).toFloat(),lineParts.at(3).toFloat()});
                    }else if(lineParts.at(0).compare("vn", Qt::CaseInsensitive) == 0){
                        //NORMAL
//                                vn.push_back({lineParts.at(1).toFloat(),lineParts.at(2).toFloat(),lineParts.at(3).toFloat()});
                    }else if(lineParts.at(0).compare("vt", Qt::CaseInsensitive) == 0){
                        //TEXTURE
                        vt.push_back({lineParts.at(1).toFloat(), lineParts.at(2).toFloat()});
                    }else if(lineParts.at(0).compare("f", Qt::CaseInsensitive) == 0){
                        //FACE DATA
                        // Faces must be triangles
                        Triangle tri;
                        // Vertices
                        tri.p[0] = v.at(lineParts.at(1).split("/").at(0).toInt() - 1);
                        tri.p[1] = v.at(lineParts.at(2).split("/").at(0).toInt() - 1);
                        tri.p[2] = v.at(lineParts.at(3).split("/").at(0).toInt() - 1);
                        // UV coords
                        if(!vt.empty()){
                            tri.t[0] = vt.at(lineParts.at(1).split("/").at(1).toInt() - 1);
                            tri.t[1] = vt.at(lineParts.at(2).split("/").at(1).toInt() - 1);
                            tri.t[2] = vt.at(lineParts.at(3).split("/").at(1).toInt() - 1);
                        }else{
                            flat = true;
                        }
                        // Normals
                        // Currently unused
//                                x = v.at(lineParts.at(1).split("/").at(2).toInt() - 1);
//                                x = v.at(lineParts.at(2).split("/").at(2).toInt() - 1);
//                                x = v.at(lineParts.at(3).split("/").at(2).toInt() - 1);

                        tris.push_back(tri);
                    }
                }
            }
        }else{
            return false;
        }
    }else{
        return false;
    }
    return true;
}



void Tools3D::AABB::updatePosition(Tools3D::Vector3 vec)
{
//    this->position = this->positionLocal + vec;
//    this->end = this->position + size;
    this->position += vec;
    this->end += size;
}

bool Tools3D::AABB::intersects(const Tools3D::AABB &other){
    if (position.x >= (other.position.x + other.size.x)) {
        return false;
    }
    if ((position.x + size.x) <= other.position.x) {
        return false;
    }
    if (position.y >= (other.position.y + other.size.y)) {
        return false;
    }
    if ((position.y + size.y) <= other.position.y) {
        return false;
    }
    if (position.z >= (other.position.z + other.size.z)) {
        return false;
    }
    if ((position.z + size.z) <= other.position.z) {
        return false;
    }

    return true;
}

Tools3D::Mesh Tools3D::AABB::toMesh()
{
    Tools3D::Mesh out;
    out.loadFromFile("Assets/cube.obj");
    out.scaleX(size.x);
    out.scaleY(size.y);
    out.scaleZ(size.z);

    return out;
}

/*--------------------------------------------------------------->*/
/*--------------------------------------------------------------->*/
/*--------------------------------------------------------------->*/
/*---------------------- Drawing functions ---------------------->*/
/*--------------------------------------------------------------->*/
/*--------------------------------------------------------------->*/
/*--------------------------------------------------------------->*/

// Draw a triangle using drawLine
void Tools3D::drawTri(QImage *image, Triangle tri, T2::Color8 color){
    T2::drawLine(image, tri.p[0].x, tri.p[0].y,
            tri.p[1].x, tri.p[1].y, color);
    T2::drawLine(image, tri.p[1].x, tri.p[1].y,
            tri.p[2].x, tri.p[2].y, color);
    T2::drawLine(image, tri.p[2].x, tri.p[2].y,
            tri.p[0].x, tri.p[0].y, color);
}
void Tools3D::drawTri(QImage *image, Triangle tri, T2::Color color){
    drawTri(image, tri, color.convert());
}

// Draw a filled triangle
// Taken from OneLonelyCoder game engine tutorial, slightly modified by me
void Tools3D::fillTri(QImage *image, Triangle tri, T2::Color8 color){
    auto SWAP = [](int &x, int &y) { int t = x; x = y; y = t; }; // Swaps the two values

    // Split the triangle into points
    int x1 = tri.p[0].x;
    int x2 = tri.p[1].x;
    int x3 = tri.p[2].x;
    int y1 = tri.p[0].y;
    int y2 = tri.p[1].y;
    int y3 = tri.p[2].y;
//    int z1 = tri.p[0].z;
//    int z2 = tri.p[1].z;
//    int z3 = tri.p[2].z;

    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;
    int signx1, signx2, dx1, dy1, dx2, dy2;
    int e1, e2;
    // Sort vertices
    if (y1>y2) { SWAP(y1, y2); SWAP(x1, x2); }
    if (y1>y3) { SWAP(y1, y3); SWAP(x1, x3); }
    if (y2>y3) { SWAP(y2, y3); SWAP(x2, x3); }

    t1x = t2x = x1; y = y1;   // Starting points
    dx1 = (int)(x2 - x1); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y2 - y1);

    dx2 = (int)(x3 - x1); if (dx2<0) { dx2 = -dx2; signx2 = -1; }
    else signx2 = 1;
    dy2 = (int)(y3 - y1);

    if (dy1 > dx1) {   // swap values
        SWAP(dx1, dy1);
        changed1 = true;
    }
    if (dy2 > dx2) {   // swap values
        SWAP(dy2, dx2);
        changed2 = true;
    }

    e2 = (int)(dx2 >> 1);
    // Flat top, just process the second half
    if (y1 == y2) goto next;
    e1 = (int)(dx1 >> 1);

    for (int i = 0; i < dx1;) {
        t1xp = 0; t2xp = 0;
        if (t1x<t2x) { minx = t1x; maxx = t2x; }
        else { minx = t2x; maxx = t1x; }
        // process first line until y value is about to change
        while (i<dx1) {
            i++;
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) t1xp = signx1;//t1x += signx1;
                else          goto next1;
            }
            if (changed1) break;
            else t1x += signx1;
        }
        // Move line
    next1:
        // process second line until y value is about to change
        while (1) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;//t2x += signx2;
                else          goto next2;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
    next2:
        if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
        if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;
        for(int i = minx; i <= maxx; i++){
            T2::drawPixel(image, i, y, color);
        }
//        drawline(minx, maxx, y);    // Draw line from min to max points found on the y
                                     // Now increase y
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y == y2) break;

    }
next:
    // Second half
    dx1 = (int)(x3 - x2); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y3 - y2);
    t1x = x2;

    if (dy1 > dx1) {   // swap values
        SWAP(dy1, dx1);
        changed1 = true;
    }
    else changed1 = false;

    e1 = (int)(dx1 >> 1);

    for (int i = 0; i <= dx1; i++) {
        t1xp = 0; t2xp = 0;
        if (t1x<t2x) { minx = t1x; maxx = t2x; }
        else { minx = t2x; maxx = t1x; }
        // process first line until y value is about to change
        while (i<dx1) {
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) { t1xp = signx1; break; }//t1x += signx1;
                else          goto next3;
            }
            if (changed1) break;
            else   	   	  t1x += signx1;
            if (i<dx1) i++;
        }
    next3:
        // process second line until y value is about to change
        while (t2x != x3) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;
                else          goto next4;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
    next4:

        if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
        if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;
        for(int i = minx; i <= maxx; i++){
            T2::drawPixel(image, i, y, color);
        }
//        drawline(minx, maxx, y);
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y>y3) return;
    }
}
void Tools3D::fillTri(QImage *image, Triangle tri, T2::Color color){
    fillTri(image, tri, color.convert());
}

void Tools3D::textureTri(QImage *image, Triangle tri, QImage *texture, std::vector<float> &dBuffer){
    // Split triangle into values for easier access
    int x1 = tri.p[0].x;
    int x2 = tri.p[1].x;
    int x3 = tri.p[2].x;
    int y1 = tri.p[0].y;
    int y2 = tri.p[1].y;
    int y3 = tri.p[2].y;
    float u1 = tri.t[0].u;
    float u2 = tri.t[1].u;
    float u3 = tri.t[2].u;
    float v1 = tri.t[0].v;
    float v2 = tri.t[1].v;
    float v3 = tri.t[2].v;
    float w1 = tri.t[0].w;
    float w2 = tri.t[1].w;
    float w3 = tri.t[2].w;


    // Sort points and their corresponding uv values
    if(y2 < y1){
        std::swap(y1, y2);
        std::swap(x1, x2);
        std::swap(u1, u2);
        std::swap(v1, v2);
        std::swap(w1, w2);
    }
    if(y3 < y1){
        std::swap(y1, y3);
        std::swap(x1, x3);
        std::swap(u1, u3);
        std::swap(v1, v3);
        std::swap(w1, w3);
    }
    if(y3 < y2){
        std::swap(y2, y3);
        std::swap(x2, x3);
        std::swap(u2, u3);
        std::swap(v2, v3);
        std::swap(w2, w3);
    }

    // Calculate triangle sides
    int dy1 = y2 - y1;
    int dx1 = x2 - x1;
    float dv1 = v2 - v1;
    float du1 = u2 - u1;
    float dw1 = w2 - w1;

    int dy2 = y3 - y1;
    int dx2 = x3 - x1;
    float dv2 = v3 - v1;
    float du2 = u3 - u1;
    float dw2 = w3 - w1;

    float texU, texV, texW;
    float daxStep = 0, dbxStep = 0, du1Step = 0, dv1Step = 0, du2Step = 0, dv2Step = 0, dw1Step = 0, dw2Step = 0;

    if (dy1) daxStep = dx1 / (float)abs(dy1);
    if (dy2) dbxStep = dx2 / (float)abs(dy2);

    if (dy1) du1Step = du1 / (float)abs(dy1);
    if (dy1) dv1Step = dv1 / (float)abs(dy1);
    if (dy1) dw1Step = dw1 / (float)abs(dy1);

    if (dy2) du2Step = du2 / (float)abs(dy2);
    if (dy2) dv2Step = dv2 / (float)abs(dy2);
    if (dy2) dw2Step = dw2 / (float)abs(dy2);

    // Calculate starting and ending x coords of each line inside the triangle
    // First loop goes vertically from y1 to y2
    if(dy1){
        for(int i = y1; i <= y2; i++){
            int ax = x1 + (float)(i - y1) * daxStep;
            int bx = x1 + (float)(i - y1) * dbxStep;

            float texSu = u1 + (float)(i - y1) * du1Step;
            float texSv = v1 + (float)(i - y1) * dv1Step;
            float texSw = w1 + (float)(i - y1) * dw1Step;

            float texEu = u1 + (float)(i - y1) * du2Step;
            float texEv = v1 + (float)(i - y1) * dv2Step;
            float texEw = w1 + (float)(i - y1) * dw2Step;

            if(ax > bx){
                std::swap(ax, bx);
                std::swap(texSu, texEu);
                std::swap(texSv, texEv);
                std::swap(texSw, texEw);
            }

            texU = texSu;
            texV = texSv;
            texW = texSw;

            float tStep = 1.0f / ((float)(bx - ax));
            float t = 0.0f;

            for(int j = ax; j < bx; j++){
                texU = (1.0f - t) * texSu + t * texEu;
                texV = (1.0f - t) * texSv + t * texEv;
                texW = (1.0f - t) * texSw + t * texEw;
                // Apply perspective to UV coords
                texU /= texW;
                texV /= texW;
                // Flip texV upside down
                texV = abs(1.0f - texV);
                // Scale UV coords to texture size
                texU *= (texture->width() - 1);
                texV *= (texture->height() - 1);

                if(texW > dBuffer[i*image->width() + j]){
                    // Get the required pixel from the texture
//                    T2::Color8 color = T2::getPixel(texture, texU, texV);
                    T2::Color8 color;

                    uchar *pixIn = texture->scanLine((int)texV);
                    int adrIn = 4 * (int)texU;
                    color.b = pixIn[adrIn];
                    color.g = pixIn[adrIn+1];
                    color.r = pixIn[adrIn+2];
//                    if(/*color.valid()*/1){
////                        T2::drawPixel(image, j, i, T2::Color(texU, texV, texW));
////                        T2::drawPixel(image, j, i, color);
//                        // Inlining the drawing function improves performance by approx. 3 times
//                        uchar *pix = image->scanLine(i);
//                        int adr = 4 * j;
//                        pix[adr] = color.b;
//                        pix[adr+1] = color.g;
//                        pix[adr+2] = color.r;
//                        pix[adr+3] = 255;
//                    }else{
//                        // Draw cyan if the color is not a valid color
//                        // Failsafe in case of bad UV mapping or other issues
//                        T2::drawPixel(image, j, i, T2::Color8(0, 255, 255));
//                    }
                    // Inlining the drawing function improves performance by approx. 3 times
                    uchar *pixOut = image->scanLine(i);
                    int adrOut = 4 * j;
                    pixOut[adrOut] = color.b;
                    pixOut[adrOut+1] = color.g;
                    pixOut[adrOut+2] = color.r;
                    pixOut[adrOut+3] = 255;
                    dBuffer[i*image->width() + j] = texW;
                }
                t += tStep;
            }
        }
    }

    dy1 = y3 - y2;
    dx1 = x3 - x2;
    dv1 = v3 - v2;
    du1 = u3 - u2;
    dw1 = w3 - w2;

    if (dy1) daxStep = dx1 / (float)abs(dy1);
    if (dy2) dbxStep = dx2 / (float)abs(dy2);

    du1Step = 0, dv1Step = 0;
    if (dy1) du1Step = du1 / (float)abs(dy1);
    if (dy1) dv1Step = dv1 / (float)abs(dy1);
    if (dy1) dw1Step = dw1 / (float)abs(dy1);

    // Second loop goes from y2 to y3
    // Mostly the same as second loop
    if(dy1){
        //ZMIENIĆ I NA FLOAT
        for(int i = y2; i <= y3; i++){
            int ax = x2 + (float)(i - y2) * daxStep;
            int bx = x1 + (float)(i - y1) * dbxStep;

            float texSu = u2 + (float)(i - y2) * du1Step;
            float texSv = v2 + (float)(i - y2) * dv1Step;
            float texSw = w2 + (float)(i - y2) * dw1Step;

            float texEu = u1 + (float)(i - y1) * du2Step;
            float texEv = v1 + (float)(i - y1) * dv2Step;
            float texEw = w1 + (float)(i - y1) * dw2Step;

            if(ax > bx){
                std::swap(ax, bx);
                std::swap(texSu, texEu);
                std::swap(texSv, texEv);
                std::swap(texSw, texEw);
            }

            texU = texSu;
            texV = texSv;
            texW = texSw;

            float tStep = 1.0f / ((float)(bx - ax));
            float t = 0.0f;

            for(int j = ax; j < bx; j++){
                texU = (1.0f - t) * texSu + t * texEu;
                texV = (1.0f - t) * texSv + t * texEv;
                texW = (1.0f - t) * texSw + t * texEw;
                // Apply perspective to uv coords
                texU /= texW;
                texV /= texW;
                // Flip texV upside down
                texV = abs(1.0f - texV);
                // Scale UV coordinates to texture size
                texU *= (texture->width() - 1);
                texV *= (texture->height() - 1);
                // Bitwise operation hack
                // Works only if the texture dimensions are a power of 2
//                unsigned int u = (unsigned int)texU & 255;

                if(texW > dBuffer[i*image->width() + j]){
                    // Get the required pixel from the texture
//                    T2::Color8 color = T2::getPixel(texture, texU, texV);
                    T2::Color8 color;

                    uchar *pixIn = texture->scanLine((int)texV);
                    int adrIn = 4 * (int)texU;
                    color.b = pixIn[adrIn];
                    color.g = pixIn[adrIn+1];
                    color.r = pixIn[adrIn+2];
//                    if(/*color.valid()*/1){
////                        T2::drawPixel(image, j, i, T2::Color(texU, texV, texW));
////                        T2::drawPixel(image, j, i, color);
//                        // Inlining the drawing function improves performance by approx. 3 times
//                        uchar *pix = image->scanLine(i);
//                        int adr = 4 * j;
//                        pix[adr] = color.b;
//                        pix[adr+1] = color.g;
//                        pix[adr+2] = color.r;
//                        pix[adr+3] = 255;
//                    }else{
//                        // Draw cyan if the color is not a valid color
//                        // Failsafe in case of bad UV mapping or other issues
//                        T2::drawPixel(image, j, i, T2::Color8(0, 255, 255));
//                    }
                    // Inlining the drawing function improves performance by approx. 3 times
                    uchar *pixOut = image->scanLine(i);
                    int adrOut = 4 * j;
                    pixOut[adrOut] = color.b;
                    pixOut[adrOut+1] = color.g;
                    pixOut[adrOut+2] = color.r;
                    pixOut[adrOut+3] = 255;
                    dBuffer[i*image->width() + j] = texW;
                }
                t += tStep;
            }
        }
    }
}

//void Tools3D::textureTri(QImage *image, Tools3D::Triangle tri, QImage *texture, std::vector<std::pair<float, Tools::Color8> > &dBuffer){
//// Split triangle into values for easier access
//int x1 = tri.p[0].x;
//int x2 = tri.p[1].x;
//int x3 = tri.p[2].x;
//int y1 = tri.p[0].y;
//int y2 = tri.p[1].y;
//int y3 = tri.p[2].y;
//float u1 = tri.t[0].u;
//float u2 = tri.t[1].u;
//float u3 = tri.t[2].u;
//float v1 = tri.t[0].v;
//float v2 = tri.t[1].v;
//float v3 = tri.t[2].v;
//float w1 = tri.t[0].w;
//float w2 = tri.t[1].w;
//float w3 = tri.t[2].w;


//// Sort points and their corresponding uv values
//if(y2 < y1){
//    std::swap(y1, y2);
//    std::swap(x1, x2);
//    std::swap(u1, u2);
//    std::swap(v1, v2);
//    std::swap(w1, w2);
//}
//if(y3 < y1){
//    std::swap(y1, y3);
//    std::swap(x1, x3);
//    std::swap(u1, u3);
//    std::swap(v1, v3);
//    std::swap(w1, w3);
//}
//if(y3 < y2){
//    std::swap(y2, y3);
//    std::swap(x2, x3);
//    std::swap(u2, u3);
//    std::swap(v2, v3);
//    std::swap(w2, w3);
//}

//// Calculate triangle sides
//int dy1 = y2 - y1;
//int dx1 = x2 - x1;
//float dv1 = v2 - v1;
//float du1 = u2 - u1;
//float dw1 = w2 - w1;

//int dy2 = y3 - y1;
//int dx2 = x3 - x1;
//float dv2 = v3 - v1;
//float du2 = u3 - u1;
//float dw2 = w3 - w1;

//float texU, texV, texW;
//float daxStep = 0, dbxStep = 0, du1Step = 0, dv1Step = 0, du2Step = 0, dv2Step = 0, dw1Step = 0, dw2Step = 0;

//if (dy1) daxStep = dx1 / (float)abs(dy1);
//if (dy2) dbxStep = dx2 / (float)abs(dy2);

//if (dy1) du1Step = du1 / (float)abs(dy1);
//if (dy1) dv1Step = dv1 / (float)abs(dy1);
//if (dy1) dw1Step = dw1 / (float)abs(dy1);

//if (dy2) du2Step = du2 / (float)abs(dy2);
//if (dy2) dv2Step = dv2 / (float)abs(dy2);
//if (dy2) dw2Step = dw2 / (float)abs(dy2);

//// Calculate starting and ending x coords of each line inside the triangle
//// First loop goes vertically from y1 to y2
//if(dy1){
//    for(int i = y1; i <= y2; i++){
//        int ax = x1 + (float)(i - y1) * daxStep;
//        int bx = x1 + (float)(i - y1) * dbxStep;

//        float texSu = u1 + (float)(i - y1) * du1Step;
//        float texSv = v1 + (float)(i - y1) * dv1Step;
//        float texSw = w1 + (float)(i - y1) * dw1Step;

//        float texEu = u1 + (float)(i - y1) * du2Step;
//        float texEv = v1 + (float)(i - y1) * dv2Step;
//        float texEw = w1 + (float)(i - y1) * dw2Step;

//        if(ax > bx){
//            std::swap(ax, bx);
//            std::swap(texSu, texEu);
//            std::swap(texSv, texEv);
//            std::swap(texSw, texEw);
//        }

//        texU = texSu;
//        texV = texSv;
//        texW = texSw;

//        float tStep = 1.0f / ((float)(bx - ax));
//        float t = 0.0f;

//        for(int j = ax; j < bx; j++){
//            texU = (1.0f - t) * texSu + t * texEu;
//            texV = (1.0f - t) * texSv + t * texEv;
//            texW = (1.0f - t) * texSw + t * texEw;
//            // Apply perspective to UV coords
//            texU /= texW;
//            texV /= texW;
//            // Flip texV upside down
//            texV = abs(1.0f - texV);
//            // Scale UV coords to texture size
//            texU *= (texture->width() - 1);
//            texV *= (texture->height() - 1);

//            if(texW > dBuffer[i*image->width() + j]){
//                T2::Color8 color = T2::getPixel(texture, texU, texV);
//                if(color.valid()){
//                    T2::drawPixel(image, j, i, color);
//                }else{
//                    // Draw cyan if the color is not a valid color
//                    // Failsafe in case of bad UV mapping or other issues
//                    T2::drawPixel(image, j, i, T2::Color8(0, 255, 255));
//                }
//                dBuffer[i*image->width() + j] = texW;
//            }


//            t += tStep;
//            }

//        }
//}

//dy1 = y3 - y2;
//dx1 = x3 - x2;
//dv1 = v3 - v2;
//du1 = u3 - u2;
//dw1 = w3 - w2;

//if (dy1) daxStep = dx1 / (float)abs(dy1);
//if (dy2) dbxStep = dx2 / (float)abs(dy2);

//du1Step = 0, dv1Step = 0;
//if (dy1) du1Step = du1 / (float)abs(dy1);
//if (dy1) dv1Step = dv1 / (float)abs(dy1);
//if (dy1) dw1Step = dw1 / (float)abs(dy1);

//// Second loop goes from y2 to y3
//// Mostly the same as second loop
//if(dy1){
//    for(int i = y2; i <= y3; i++){
//        int ax = x2 + (float)(i - y2) * daxStep;
//        int bx = x1 + (float)(i - y1) * dbxStep;

//        float texSu = u2 + (float)(i - y2) * du1Step;
//        float texSv = v2 + (float)(i - y2) * dv1Step;
//        float texSw = w2 + (float)(i - y2) * dw1Step;

//        float texEu = u1 + (float)(i - y1) * du2Step;
//        float texEv = v1 + (float)(i - y1) * dv2Step;
//        float texEw = w1 + (float)(i - y1) * dw2Step;

//        if(ax > bx){
//            std::swap(ax, bx);
//            std::swap(texSu, texEu);
//            std::swap(texSv, texEv);
//            std::swap(texSw, texEw);
//        }

//        texU = texSu;
//        texV = texSv;
//        texW = texSw;

//        float tStep = 1.0f / ((float)(bx - ax));
//        float t = 0.0f;

//        for(int j = ax; j < bx; j++){
//            texU = (1.0f - t) * texSu + t * texEu;
//            texV = (1.0f - t) * texSv + t * texEv;
//            texW = (1.0f - t) * texSw + t * texEw;
//            // Apply perspective to uv coords
//            texU /= texW;
//            texV /= texW;
//            // Flip texV upside down
//            texV = abs(1.0f - texV);
//            // Scale UV coordinates to texture size
//            texU *= (texture->width() - 1);
//            texV *= (texture->height() - 1);

//            if(texW > dBuffer[i*image->width() + j]){
//                T2::Color8 color = T2::getPixel(texture, texU, texV);
//                if(color.valid()){
//                    T2::drawPixel(image, j, i, color);
//                }else{
//                    // Draw cyan if the color is not a valid color
//                    // Failsafe in case of bad UV mapping or other issues
//                    T2::drawPixel(image, j, i, T2::Color8(0, 255, 255));
//                }
//                dBuffer[i*image->width() + j] = texW;
//            }
//        }
//    }
//}
//}


/*------------------------------------------------------------*/
/*------------------------------------------------------------*/
/*---------------------- Math functions ----------------------*/
/*------------------------------------------------------------*/
/*------------------------------------------------------------*/

float Tools3D::dotProduct(Vector3 v1, Vector3 v2){
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
float Tools3D::length(Vector3 v){
    return sqrtf(dotProduct(v, v));
}
Tools3D::Vector3 Tools3D::normalise(Vector3 v){
    float l = length(v); // Length of the input vector
    return {v.x/l, v.y/l, v.z/l};
}
Tools3D::Vector3 Tools3D::crossProduct(Vector3 v1, Vector3 v2){
    float Nx = v1.y * v2.z - v1.z * v2.y;
    float Ny = v1.z * v2.x - v1.x * v2.z;
    float Nz = v1.x * v2.y - v1.y * v2.x;
    return {Nx, Ny, Nz};
}

Tools3D::Mat4x4 Tools3D::newMatIdentity(){
    Mat4x4 out;
    out.m[0][0] = 1.0f;
    out.m[1][1] = 1.0f;
    out.m[2][2] = 1.0f;
    out.m[3][3] = 1.0f;
    return out;
}
// Create a matrix for rotating in x/y/z axis by the specified angle in radians
Tools3D::Mat4x4 Tools3D::newMatRotX(float angle){
    Mat4x4 mat;
    mat.m[0][0] = 1;
    mat.m[1][1] = cosf(angle);
    mat.m[1][2] = sinf(angle);
    mat.m[2][1] = -sinf(angle);
    mat.m[2][2] = cosf(angle);
    mat.m[3][3] = 1;
    return mat;
}
Tools3D::Mat4x4 Tools3D::newMatRotY(float angle){
    Mat4x4 mat;
    mat.m[0][0] = cosf(angle);
    mat.m[0][2] = sinf(angle);
    mat.m[2][0] = -sinf(angle);
    mat.m[1][1] = 1.0f;
    mat.m[2][2] = cosf(angle);
    mat.m[3][3] = 1.0f;
    return mat;
}
Tools3D::Mat4x4 Tools3D::newMatRotZ(float angle){
    Mat4x4 mat;
    mat.m[0][0] = cosf(angle);
    mat.m[0][1] = sinf(angle);
    mat.m[1][0] = -sinf(angle);
    mat.m[1][1] = cosf(angle);
    mat.m[2][2] = 1;
    mat.m[3][3] = 1;
    return mat;
}
// Translation matrix
Tools3D::Mat4x4 Tools3D::newMatTrans(float x, float y, float z){
    Mat4x4 mat;
    mat.m[0][0] = 1.0f;
    mat.m[1][1] = 1.0f;
    mat.m[2][2] = 1.0f;
    mat.m[3][3] = 1.0f;
    mat.m[3][0] = x;
    mat.m[3][1] = y;
    mat.m[3][2] = z;
    return mat;
}
// Creates a projection matrix.
// fovDeg is the field of vision in degrees
// near/fear determine the view frustrum
Tools3D::Mat4x4 Tools3D::newMatProj(float fovDeg, float aspectRatio, float near, float far){
    Mat4x4 mat;
    float fovRad = 1.0f / tanf(fovDeg * 0.5f / 180.0f * 3.14159f);
    mat.m[0][0] = aspectRatio * fovRad;
    mat.m[1][1] = fovRad;
    mat.m[2][2] = far / (far - near);
    mat.m[3][2] = (-far * near) / (far - near);
    mat.m[2][3] = 1.0f;
    mat.m[3][3] = 0.0f;
    return mat;
}

Tools3D::Mat4x4 Tools3D::matPointAt(Vector3 pos, Vector3 target, Vector3 up){
    // Calculate the new directions
    // Forward
    Vector3 newForward = target - pos;
    newForward = normalise(newForward);

    // Up
    Vector3 a = newForward * dotProduct(up, newForward);
    Vector3 newUp = up - a;
    newUp = normalise(newUp);

    // Right
    Vector3 newRight = crossProduct(newUp, newForward);

    // Dimensioning and translation matrix
    Mat4x4 mat;
    mat.m[0][0] = newRight.x;   mat.m[0][1] = newRight.y;   mat.m[0][2] = newRight.z;   mat.m[0][3] = 0;
    mat.m[1][0] = newUp.x;      mat.m[1][1] = newUp.y;      mat.m[1][2] = newUp.z;      mat.m[1][3] = 0;
    mat.m[2][0] = newForward.x; mat.m[2][1] = newForward.y; mat.m[2][2] = newForward.z; mat.m[2][3] = 0;
    mat.m[3][0] = pos.x;        mat.m[3][1] = pos.y;        mat.m[3][2] = pos.z;        mat.m[3][3] = 1.0f;

    return mat;
}
// Works only for rotation/translation matrices, i.e. the one returned by matPointAt()
Tools3D::Mat4x4 Tools3D::matQuickInverse(Mat4x4 in){
    Mat4x4 out;
    out.m[0][0] = in.m[0][0]; out.m[0][1] = in.m[1][0]; out.m[0][2] = in.m[2][0];
    out.m[1][0] = in.m[0][1]; out.m[1][1] = in.m[1][1]; out.m[1][2] = in.m[2][1];
    out.m[2][0] = in.m[0][2]; out.m[2][1] = in.m[1][2]; out.m[2][2] = in.m[2][2];
    out.m[3][0] = -(in.m[3][0] * out.m[0][0] + in.m[3][1] * out.m[1][0] + in.m[3][2] * out.m[2][0]);
    out.m[3][1] = -(in.m[3][0] * out.m[0][1] + in.m[3][1] * out.m[1][1] + in.m[3][2] * out.m[2][1]);
    out.m[3][2] = -(in.m[3][0] * out.m[0][2] + in.m[3][1] * out.m[1][2] + in.m[3][2] * out.m[2][2]);
    out.m[3][3] = 1.0f;
    return out;
}

// Returns the point at which a line interescts with a plane
Tools3D::Vector3 Tools3D::intersectPlane(Vector3 planePoint, Vector3 planeNormal, Vector3 lineStart, Vector3 lineEnd, float &t){
    planeNormal = normalise(planeNormal);
    float planeDotP = -dotProduct(planeNormal, planePoint);
    float startDotP = dotProduct(lineStart, planeNormal);
    float endDotP = dotProduct(lineEnd, planeNormal);
    t = (-planeDotP - startDotP) / (endDotP - startDotP);
    Vector3 lineStartToEnd = lineEnd - lineStart;
    Vector3 lineToIntersect = lineStartToEnd * t;
    return lineStart + lineToIntersect;
}

int Tools3D::clipTriangle(Vector3 planePoint, Vector3 planeNormal, Triangle inTri, Triangle &outTri1, Triangle &outTri2){
    // Make sure the plane normal is normal
    planeNormal = normalise(planeNormal);

    // Return signed shortest distance from point to plane
    auto dist = [&](Vector3 &p){
        Vector3 n = normalise(p);
//        return (planeNormal.x * n.x + planeNormal.y * n.y + planeNormal.z * n.z - dotProduct(planeNormal, planePoint));
        return (planeNormal.x * p.x + planeNormal.y * p.y + planeNormal.z * p.z - dotProduct(planeNormal, planePoint));
    };

    // Create two temporary storage arrays to classify points either side of the plane
    // If distance sign is positive, point lies on the "inside" of the plane
    Vector3* insidePoints[3]; int insidePointCount = 0;
    Vector3* outsidePoints[3]; int outsidePointCount = 0;
    UV* insideTex[3]; int insideTexCount = 0;
    UV* outsideTex[3]; int outsideTexCount = 0;

    //Get distance of each point in triangle to plane
    float d0 = dist(inTri.p[0]);
    float d1 = dist(inTri.p[1]);
    float d2 = dist(inTri.p[2]);

    if(d0 >= 0){ insidePoints[insidePointCount++] = &inTri.p[0]; insideTex[insideTexCount++] = &inTri.t[0];}
    else{ outsidePoints[outsidePointCount++] = &inTri.p[0]; outsideTex[outsideTexCount++] = &inTri.t[0];}
    if(d1 >= 0){ insidePoints[insidePointCount++] = &inTri.p[1]; insideTex[insideTexCount++] = &inTri.t[1];}
    else{ outsidePoints[outsidePointCount++] = &inTri.p[1]; outsideTex[outsideTexCount++] = &inTri.t[1];}
    if(d2 >= 0){ insidePoints[insidePointCount++] = &inTri.p[2]; insideTex[insideTexCount++] = &inTri.t[2];}
    else{ outsidePoints[outsidePointCount++] = &inTri.p[2]; outsideTex[outsideTexCount++] = &inTri.t[2];}

    // Classify triangle points and split triangles into smaller triangles if necessary

    // All points are on the outside, the triangle is removed
    if(insidePointCount == 0){
        return 0; // No triangles are valid
    }
    // All points are on the inside, the triangle is preserved completly
    if(insidePointCount == 3){
        outTri1 = inTri;
        return 1; // One triangle is valid
    }
    // One point is on the inside, the triangle is clipped into a smaller triangle
    if(insidePointCount == 1 && outsidePointCount == 2){
        // Copy triangle color/texture data into the new triangle
        outTri1.color = inTri.color;

        // Contruct the output triangle
        // One point is valid, so it doesn't need to be calculated
        outTri1.p[0] = *insidePoints[0];
        outTri1.t[0] = *insideTex[0];
        // The other two points are on the intersecting plane
        float t;
        outTri1.p[1] = intersectPlane(planePoint, planeNormal, *insidePoints[0], *outsidePoints[0], t);
        outTri1.t[1].u = t * (outsideTex[0]->u - insideTex[0]->u) + insideTex[0]->u;
        outTri1.t[1].v = t * (outsideTex[0]->v - insideTex[0]->v) + insideTex[0]->v;
        outTri1.t[1].w = t * (outsideTex[0]->w - insideTex[0]->w) + insideTex[0]->w;

        outTri1.p[2] = intersectPlane(planePoint, planeNormal, *insidePoints[0], *outsidePoints[1], t);
        outTri1.t[2].u = t * (outsideTex[1]->u - insideTex[0]->u) + insideTex[0]->u;
        outTri1.t[2].v = t * (outsideTex[1]->v - insideTex[0]->v) + insideTex[0]->v;
        outTri1.t[2].w = t * (outsideTex[1]->w - insideTex[0]->w) + insideTex[0]->w;

        return 1; // One triangle is valid
    }
    // Two points are on the inside, the triangle is clipped and split into two smaller triangles
    if(insidePointCount == 2 && outsidePointCount == 1){
        // Copy triangle color/texture data into the new triangles
        outTri1.color = inTri.color;
        outTri2.color = inTri.color;

        // Contruct the first output triangle
        // It consists of the two inside points,
        // and a new point determined by the point where one side of the triangle intersects with the plane
        outTri1.p[0] = *insidePoints[0];
        outTri1.t[0] = *insideTex[0];
        outTri1.p[1] = *insidePoints[1];
        outTri1.t[1] = *insideTex[1];

        float t;
        outTri1.p[2] = intersectPlane(planePoint, planeNormal, *insidePoints[0], *outsidePoints[0], t);
        outTri1.t[2].u = t * (outsideTex[0]->u - insideTex[0]->u) + insideTex[0]->u;
        outTri1.t[2].v = t * (outsideTex[0]->v - insideTex[0]->v) + insideTex[0]->v;
        outTri1.t[2].w = t * (outsideTex[0]->w - insideTex[0]->w) + insideTex[0]->w;

        // Contruct the second output triangle
        // It consists of one inside point,
        // the new point created for the first triangle,
        // and a new point determined by the intersection of the other side of the triangle and the plane
        outTri2.p[0] = *insidePoints[1];
        outTri2.t[0] = *insideTex[1];
        outTri2.p[1] = outTri1.p[2];
        outTri2.t[1] = outTri1.t[2];
        outTri2.p[2] = intersectPlane(planePoint, planeNormal, *insidePoints[1], *outsidePoints[0], t);
        outTri2.t[2].u = t * (outsideTex[0]->u - insideTex[1]->u) + insideTex[1]->u;
        outTri2.t[2].v = t * (outsideTex[0]->v - insideTex[1]->v) + insideTex[1]->v;
        outTri2.t[2].w = t * (outsideTex[0]->w - insideTex[1]->w) + insideTex[1]->w;

        return 2; // Two triangles are valid
    }

    qDebug("CLIP TRIANGLE REACHED AN UNEXPETED END");
}
