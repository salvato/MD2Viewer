TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt


INCLUDEPATH += ./Headers
INCLUDEPATH += ../external/glfw-3.1.2/include/
INCLUDEPATH += ../external/glm-0.9.9.3/
INCLUDEPATH += ../external/glew-1.13.0/include
INCLUDEPATH += ../external/stb/
# For The Bullet Library
INCLUDEPATH += /usr/include/bullet


LIBS += -L/usr/lib
LIBS += -L../build/external
LIBS += -L../build/external/glfw-3.1.2/src


LIBS += -lglfw3
LIBS += -lX11
LIBS += -lGLEW_1130
LIBS += -lpthread
LIBS += -lGL
LIBS += -ldl
LIBS += -lXxf86vm
LIBS += -lXrandr
LIBS += -lXinerama
LIBS += -lXcursor
LIBS += -lXi
# For The Bullet Library
LIBS += -lBulletDynamics
LIBS += -lBulletCollision
LIBS += -lLinearMath



SOURCES += main.cpp \
    Game.cpp \
    Graphics.cpp \
    ObjectModel.cpp \
    MyFiles.cpp \
    ModelManager.cpp \
    Font3D.cpp \
    ShaderManager.cpp \
    TextureManager.cpp \
    OffsetFollow.cpp \
    Ground.cpp \
    BinaryReader.cpp \
    TinyObjectLoader.cpp \
    Camera.cpp \
    Frustum.cpp \
    OBJModel.cpp \
    Player.cpp \
    MD2Anim.cpp \
    MD2Model.cpp \
    CollisionProcess.cpp \
    EnemyManager.cpp \
    PhysicsDraw.cpp \
    PhysicsObj.cpp \
    Rays.cpp


HEADERS += \
    Game.h \
    Graphics.h \
    ObjectModel.h \
    MyFiles.h \
    ModelManager.h \
    Font3D.h \
    ShaderManager.h \
    TextureManager.h \
    OffsetFollow.h \
    Ground.h \
    BinaryReader.h \
    tiny_obj_loader.h \
    Camera.h \
    Frustum.h \
    OBJModel.h \
    Player.h \
    MD2.h \
    MD2Anim.h \
    MD2Model.h \
    CollisionProcess.h \
    Enemies.h \
    EnemyManager.h \
    PhysicsDraw.h \
    PhysicsObj.h \
    Rays.h

RESOURCES +=

DISTFILES +=
