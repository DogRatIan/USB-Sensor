# Add more folders to ship with the application, here
#folder_01.source = qml
#folder_01.target = qml
#DEPLOYMENTFOLDERS = folder_01
DEPLOYMENTFOLDERS =
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += src/main.cpp \
    src/usb_cdc.cpp \
    src/control_top.cpp \
    src/worker.cpp \
    src/qml_utils.cpp

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2controlsapplicationviewer/qtquick2controlsapplicationviewer.pri)
qtcAddDeployment()

OTHER_FILES += \
    qml/main.qml \
    android-sources/src/com/dogratian/JavaCdc.java \
    android-sources/AndroidManifest.xml \
    qml/dogratian/ImageButton.qml

HEADERS += \
    src/usb_cdc.h \
    src/control_top.h \
    src/worker.h \
    src/qml_utils.h

android {QT += androidextras}
windows {QT += serialport}

RESOURCES += \
    main.qrc
