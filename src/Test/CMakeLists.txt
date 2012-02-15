set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-zero-as-null-pointer-constant")

corrade_add_test(ObjectTest ObjectTest.h ObjectTest.cpp Magnum)
corrade_add_test(CameraTest CameraTest.h CameraTest.cpp Magnum)
corrade_add_test(SceneTest SceneTest.h SceneTest.cpp Magnum)
corrade_add_test(MeshBuilderTest MeshBuilderTest.h MeshBuilderTest.cpp Magnum)
