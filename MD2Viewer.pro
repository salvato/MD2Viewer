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



SOURCES += \
    Sources/BinaryReader.cpp \
    Sources/Camera.cpp \
    Sources/CollisionProcess.cpp \
    Sources/EnemyManager.cpp \
    Sources/Font3D.cpp \
    Sources/Frustum.cpp \
    Sources/Game.cpp \
    Sources/Graphics.cpp \
    Sources/Ground.cpp \
    Sources/main.cpp \
    Sources/MD2Anim.cpp \
    Sources/MD2Model.cpp \
    Sources/ModelManager.cpp \
    Sources/MyFiles.cpp \
    Sources/ObjectModel.cpp \
    Sources/OBJModel.cpp \
    Sources/OffsetFollow.cpp \
    Sources/PhysicsDraw.cpp \
    Sources/PhysicsObj.cpp \
    Sources/Player.cpp \
    Sources/Quad.cpp \
    Sources/Rays.cpp \
    Sources/ShaderManager.cpp \
    Sources/TextureManager.cpp \
    Sources/TinyObjectLoader.cpp

HEADERS += \
    Headers/BinaryReader.h \
    Headers/Camera.h \
    Headers/CollisionProcess.h \
    Headers/Enemies.h \
    Headers/EnemyManager.h \
    Headers/Font3D.h \
    Headers/Frustum.h \
    Headers/Game.h \
    Headers/Graphics.h \
    Headers/Ground.h \
    Headers/GroundPlane.h \
    Headers/MD2.h \
    Headers/MD2Anim.h \
    Headers/MD2Model.h \
    Headers/ModelManager.h \
    Headers/MyFiles.h \
    Headers/ObjectModel.h \
    Headers/OBJModel.h \
    Headers/OffsetFollow.h \
    Headers/PhysicsDraw.h \
    Headers/PhysicsObj.h \
    Headers/Player.h \
    Headers/Quad.h \
    Headers/Rays.h \
    Headers/ShaderManager.h \
    Headers/stb_image.h \
    Headers/TextureManager.h \
    Headers/tiny_obj_loader.h

RESOURCES += \
    md2viewer.qrc

DISTFILES +=
