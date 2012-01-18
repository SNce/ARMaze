#ifndef AR_VRML97INT_H
#define AR_VRML97INT_H

//#include <vrml97/Doc.h>
#include <vrml/System.h>
#include <vrml/VrmlScene.h>
#include <vrml/ViewerOpenGL.h>

#define  AR_LIGHT_MAX     8
#define  AR_AVATAR_MAX    8

typedef struct {
    int    id;
    double scale;
    double trans[16];
} AR_AVATAR_POS_T;

typedef struct {
    int   active;
    float ambient;
    float intensity;
    float rgb[3];
    float direction[3];
} AR_DIR_LIGHT_T;

typedef struct {
    int   active;
    float ambient;
    float attenuation[3];
    float rgb[3];
    float intensity;
    float location[3];
    float radius;
} AR_POINT_LIGHT_T;

typedef struct {
    int   active;
    float ambient;
    float attenuation[3];
    float beamWidth;
    float rgb[3];
    float cutOffAngle;
    float direction[3];
    float intensity;
    float location[3];
    float radius;
} AR_SPOT_LIGHT_T;


class arVrml97Viewer : public ViewerOpenGL {

public:
    arVrml97Viewer(VrmlScene*);
   ~arVrml97Viewer();

    void timerUpdate();
    void render();

    char             filename[512];
    VrmlScene        *vrmlScene;
    double           translation[3];
    double           rotation[4];
    double           scale[3];

    int arVrml97ChangeARMode( int flag );

    int insertAvatar( int avatar_id, double scale, double trans_mat[16] );

    int arVrml97InsertDirLight( float ambient, float intensity,
                                float rgb[3], float direction[3] );
    int arVrml97InsertPointLight( float ambient, float attenuation[3],
                                  float rgb[3], float intensity,
                                  float location[3], float radius );
    int arVrml97InsertSpotLight( float ambient, float attenuation[3],
                                 float beamWidth, float rgb[3], float cutOffAngle,
                                 float direction[3], float intensity,
                                 float location[3], float radius );


    virtual void setViewpoint(float *position,
                              float *orientation,
                              float fieldOfView,
                              float avatarSize,
                              float visLimit);

    virtual Object insertBackground(int nGroundAngles = 0,
                                    float* groundAngle = 0,
                                    float* groundColor = 0,
                                    int nSkyAngles = 0,
                                    float* skyAngle = 0,
                                    float* skyColor = 0,
                                    int* whc = 0,
                                    unsigned char** pixels = 0);

    virtual void setFog(float * /*color*/,
                        float   /*visibilityRange*/,
                        const char * /*fogType*/);



protected:
    AR_AVATAR_POS_T  avatar[AR_AVATAR_MAX];

    AR_DIR_LIGHT_T   dir_light[AR_LIGHT_MAX];
    AR_POINT_LIGHT_T point_light[AR_LIGHT_MAX];
    AR_SPOT_LIGHT_T  spot_light[AR_LIGHT_MAX];

    virtual void wsPostRedraw() {};
    virtual void wsSwapBuffers() {};
    virtual void wsSetTimer( double ) {};
    void wsSetCursor( CursorStyle c);
};

#endif
