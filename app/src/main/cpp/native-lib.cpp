#include <jni.h>
#include <string>
#include <dlib/image_processing/frontal_face_detector.h>
#include "omp.h"
#include <dlib/image_processing/shape_predictor.h>
#include <android/log.h>
#include "vector"
#include "includes.h"
#include <dlib/image_io.h>
#include <iostream>
#define  LOG_TAG    "your-log-tag"

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
using namespace std;

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_eye_1detection_MainActivity_loadImages(JNIEnv *env, jobject thiz, jstring Fullpath,jstring path,jstring name) {
    // TODO: implement loadImages()
    PrintImg print;

    try {
        const char * ruta=(*env).GetStringUTFChars(Fullpath,0);
        dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
        dlib::shape_predictor sp;
        //LOGD("----------------  :V :) :( :l ---------------- ");
        dlib::deserialize("storage/emulated/0/shape_predictor_68_face_landmarks.dat") >> sp;
        //LOGD("---------------- si pasa :V deserialize ");
        dlib::array2d<dlib::rgb_pixel> img;


        dlib::load_image(img, ruta);

        std::vector<dlib::rectangle> dets = detector(img);
        int pt37[2], pt40[2], y12[2];
        std::vector<dlib::full_object_detection> shapes;
        for (unsigned long j = 0; j < dets.size(); ++j)
        {

            dlib::full_object_detection shape = sp(img, dets[j]);
            // You get the idea, you can get all the face part locations if
            // you want them.  Here we just store them in shapes so we can
            // put them on the screen.
            shapes.push_back(shape);
            pt37[0] = shape.part(37).x() - (shape.part(37).x() / 5);
            pt37[1] = shape.part(37).y();
            pt40[0] = shape.part(40).x() + (shape.part(40).x() / 5);
            pt40[1] = shape.part(40).y();
            y12[0] = shape.part(38).y() - (shape.part(38).y() / 8);
            y12[1] = shape.part(42).y() + (shape.part(42).y() / 8);
        }

        int imgRow, imgCol, imgrowE, imgecolE;
        float *auximg;
        Matrix imgface;

        imgecolE = (pt40[0] - pt37[0]);
        imgrowE = (y12[1] - y12[0]);
        LOGD("----------------  :V :) :( :l ---------------- ");
        Matrix cropeye = newDoubleMatrix(imgrowE, imgecolE);


        loadImage2(ruta, imgRow, imgCol, auximg, imgface);

        for (int y = y12[0], i = 0; y < y12[1]; y++, i++)
        {
            for (int x = pt37[0], j = 0; x < pt40[0]; x++, j++)
            {

                cropeye[i][j] = imgface[y][x];
            }
        }


        const char * nom=(*env).GetStringUTFChars(name,0);
        string nombre = nom;
        string croppath = "/storage/emulated/0/crop/"+nombre;

        const char * c = croppath.c_str();
        print.PrintImgs(cropeye, imgrowE, imgecolE,c);

        delete[] auximg;
        auximg = nullptr;
        deleteDoubleMatrix(imgface, imgRow);
        deleteDoubleMatrix(cropeye, imgrowE);


    }catch(dlib::serialization_error& e) {
        //toDo
        LOGD("---------------- Error: %s",e.what());
        std::cout << std::endl << e.what() << std::endl;
    }


    return path;

}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_eye_1detection_MainActivity_detectIris(JNIEnv *env, jobject thiz,jstring name) {
    // TODO: implement detectIris()
    PrintImg print;
    DIR *dir;
    struct dirent *diread;
    const char *path = "/storage/emulated/0/crop/";
    const char * nom=(*env).GetStringUTFChars(name,0);
    std::string nombre =nom;
    if ((dir = opendir(path)) != nullptr)
    {
        while ((diread = readdir(dir)) != nullptr)
        {

            std::string name_file = diread->d_name;
            if (name_file != "." && name_file != "..")
            {
                std::string val = path + name_file;
                LOGD("cropath : %s" ,name_file.c_str());
                LOGD("cropath : %s" ,nombre.c_str());
                if(nombre == name_file){

                    int imgRow, imgCol;
                    float *auximg;
                    Matrix img3;


                    loadImage2(val.c_str(), imgRow, imgCol, auximg, img3);


                    int outrow = imgRow, outcol = imgCol;
                    scaling(outrow, outcol, 400);
                    Matrix img = imgResize(auximg, imgRow, imgCol, outrow, outcol);
                    Matrix mat_gradient = newDoubleMatrix(outrow, outcol);
                    Matrix mat_or = newDoubleMatrix(outrow, outcol);


                    double g = 2.2;
                    double borde = 1.5;
                    double histmax = 0.34;
                    double histmin = 0.27;

                    canny(img, mat_gradient, mat_or, outrow, outcol, 1, 1, 1);
                    adjgamma(mat_gradient, outrow, outcol, g);
                    Matrix i4 = nonmaxsup(mat_gradient, outrow, outcol, mat_or, outrow, outcol, borde);
                    IntMatrix final = hysthresh(i4, outrow, outcol, histmax, histmin);



                    int r_min = 61;
                    int r_max = 105;

                    IntVector pcoor = detectar_circulo(final, outrow, outcol, r_min, r_max, 0.1);

                    //escalar el radio
                    // int raux=pcoor[2];
                    int rx, ry, r;
                    resizeExternalCoor(rx, ry, r, imgCol, outcol, imgRow, outrow, pcoor);

                    //---------------
                    deleteDoubleMatrix(mat_gradient, outrow);
                    deleteDoubleMatrix(mat_or, outrow);
                    deleteDoubleMatrix(i4, outrow);
                    deleteIntMatrix(final, outrow);

                    int fila = pcoor[1];    // x
                    int columna = pcoor[0]; // y
                    int radio = pcoor[2];   // r
                    int fil_col = radio * 2;
                    scaling(outrow, outcol, 400);
                    Matrix img2 = imgResize(auximg, imgRow, imgCol, outrow, outcol);
                    Matrix mat_ojo = newDoubleMatrix(fil_col, fil_col);
                    int conti = 0;
                    int contj = 0;
                    for (int i = fila - radio; i < fila + radio; ++i)
                    {
                        for (int j = columna - radio; j < columna + radio; ++j)
                        {
                            mat_ojo[conti][contj] = img2[i][j];
                            contj++;
                        }
                        contj = 0;
                        conti++;
                    }

                    deleteDoubleMatrix(img2, outrow);

                    Matrix mat_gradient2 = newDoubleMatrix(fil_col, fil_col);
                    Matrix mat_or2 = newDoubleMatrix(fil_col, fil_col);

                    //--ubiris session1 y session2 3.4, 1.5 0.34,0.27
                    // casia lamp 2.2,1.5,0.34,0.27
                    // prsonal ojo 3.5,1.5 , 0.34,0.27
                    double g2 = 3.4;
                    double borde2 = 1.5;
                    double histmax2 = 0.34;
                    double histmin2 = 0.27;
                    canny(mat_ojo, mat_gradient2, mat_or2, fil_col, fil_col, 1, 1, 1, 2);
                    adjgamma(mat_gradient2, fil_col, fil_col, g2);
                    Matrix i42 = nonmaxsup(mat_gradient2, fil_col, fil_col, mat_or2, fil_col, fil_col, borde2);
                    IntMatrix final2 = hysthresh(i42, fil_col, fil_col, histmax2, histmin2);

                    //----------------------
                    // ubiris session1 y sssion2 rmin=radio*0.18 rmax=radio*0.20
                    // casia lamp rmin=radio*0.18 rmax=radio*0.18
                    int r_min2 = round(radio * 0.18);
                    int r_max2 = radio - round(radio * 0.2);
                    IntVector pcoor2 = detectar_circulo(final2, fil_col, fil_col, r_min2, r_max2, 0.20, 1);

                    int rx2, ry2, r2;
                    resizeInternalCoor(rx2, ry2, r2, imgCol, outcol, imgRow, outrow, pcoor2, columna, fila, radio);

                    correctSegmentation(rx, ry, rx2, ry2, r2);

                    paintCircle(img3, imgRow, imgCol, rx, ry, rx2, ry2, r, r2);

                    int mayor = 120,acum=0,acum2=0,cont=0,cont2=0,promedio,max;
                    for (int i=0;i<imgRow;i++){
                        for(int j=0;j<imgCol;j++){
                            int d = (pow((rx-j),2))+(pow((ry-i),2));
                            int rad = pow(r,2);
                            if ( d < rad ){
                                acum=acum+img3[i][j];
                                cont=cont+1;
                                if(img3[i][j]>mayor){
                                    mayor=img3[i][j];
                                    cont2=cont2+1;
                                    acum2=acum2+mayor;
                                }}
                        }
                    }
                    if(cont2 != 0){
                        max = acum2 / cont2;
                        promedio=acum/cont;
                        int uref=promedio+0.9*(max-promedio);


                        for (int i=ry-r;i<ry+r;i++){
                            for(int j=rx-r;j<rx+r;j++){
                                int d = (pow((rx-j),2))+(pow((ry-i),2));
                                int rad = pow(r,2);
                                if ( d < rad ){
                                    if(img3[i][j] > uref){
                                        //img4[i][j]=(img4[i-15][j+15]+img4[i][j+15]+img4[i+15][j+15]+img4[i+15][j]+img4[i+15][j-15]+img4[i][j-15]+img4[i-15][j-15]+img4[i-15][j])/8;
                                        img3[i][j]=255;
                                    }
                                }
                            }
                        }
                        for (int x=rx-r ; x<rx+r ; x++){
                            for(int y=ry-r;y<ry+r;y++){
                                int d = (pow((rx-y),2))+(pow((ry-x),2));
                                int rad = pow(r,2);
                                if ( d <= rad ){
                                    if(img3[x][y] > uref ){
                                        img3[x-1][y+1]=255;
                                        img3[x-1][y]=255;
                                        img3[x-1][y-1]=255;
                                        img3[x][y-1]=255;


                                    }
                                }
                            }
                        }


                    }


                    string pat="/storage/emulated/0/iris/"+nombre;
                    const char *Lugarnombre = pat.c_str();

                    print.PrintImgs(img3, imgRow, imgCol, Lugarnombre);


                    delete[] auximg;
                    auximg = nullptr;
                    deleteIntVector(pcoor2);
                    deleteIntVector(pcoor);

                    deleteDoubleMatrix(img3, imgRow);
                    // deleteDoubleMatrix(img4,imgRow);
                    deleteDoubleMatrix(mat_gradient2, fil_col);
                    deleteDoubleMatrix(mat_or2, fil_col);
                    deleteDoubleMatrix(i42, fil_col);
                    deleteIntMatrix(final2, fil_col);
                }
            }
        }
    }


}