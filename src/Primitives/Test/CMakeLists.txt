set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-zero-as-null-pointer-constant")

corrade_add_test(CapsuleTest CapsuleTest.h CapsuleTest.cpp ${MAGNUM_PRIMITIVES_LIBRARY})
corrade_add_test(UVSphereTest UVSphereTest.h UVSphereTest.cpp ${MAGNUM_PRIMITIVES_LIBRARY})
