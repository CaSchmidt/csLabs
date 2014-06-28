TARGET_ARCH =

win32-msvc*: contains(QMAKE_TARGET.arch, x86): {
 TARGET_ARCH = -x86
}
win32-msvc*: contains(QMAKE_TARGET.arch, x86_64): {
 TARGET_ARCH = -x64
}

TARGET_POSTFIX = $${TARGET_ARCH}
CONFIG(debug, debug|release): TARGET_POSTFIX = $${TARGET_POSTFIX}_debug
