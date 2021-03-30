#include "mesh3d.h"

bool Mesh3D::loadFromFile(const QString& fileName){
    tris.clear();
    QFile file(fileName);
    if(file.exists()){
        if(file.open(QFile::ReadOnly | QFile::Text)){
            std::vector<Tools3D::Vector3> v;//, vn;
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
//                            vn.push_back({lineParts.at(1).toFloat(),lineParts.at(2).toFloat(),lineParts.at(3).toFloat()});
                    }else if(lineParts.at(0).compare("vt", Qt::CaseInsensitive) == 0){
                        //TEXTURE
//                            vt.push_back({lineParts.at(1).toFloat(), lineParts.at(2).toFloat()});
                    }else if(lineParts.at(0).compare("f", Qt::CaseInsensitive) == 0){
                        //FACE DATA
                        // Faces must be triangles
                        Tools3D::Triangle tri;
                        // Vertices
                        tri.p[0] = v.at(lineParts.at(1).split("/").at(0).toInt() - 1);
                        tri.p[1] = v.at(lineParts.at(2).split("/").at(0).toInt() - 1);
                        tri.p[2] = v.at(lineParts.at(3).split("/").at(0).toInt() - 1);
                        // UV coords
//                            if(!vt.empty()){
//                                tri.t[0] = vt.at(lineParts.at(1).split("/").at(1).toInt() - 1);
//                                tri.t[1] = vt.at(lineParts.at(2).split("/").at(1).toInt() - 1);
//                                tri.t[2] = vt.at(lineParts.at(3).split("/").at(1).toInt() - 1);
//                            }else{
//                                flat = true;
//                            }
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
void Mesh3D::scale(float mod){
    for(int tIdx = 0; tIdx < tris.size(); tIdx++){
        for(int pIdx = 0; pIdx <= 2; pIdx++){
            tris[tIdx].p[pIdx] = tris[tIdx].p[pIdx] * mod;
        }
    }
}
// Adds the specified vector to all vertices within this mesh
void Mesh3D::moveVertices(Tools3D::Vector3 v){
    for(int i = 0; i < tris.size(); i++){
        tris[i].p[0] = tris[i].p[0] + v;
        tris[i].p[1] = tris[i].p[1] + v;
        tris[i].p[2] = tris[i].p[2] + v;
    }
}
bool Mesh3D::empty(){
    return tris.empty();
}
