#-------------------------------------------------
#
# Project created by QtCreator 2012-10-11T08:27:01
#
#-------------------------------------------------

QT += core gui
QT += opengl
QT += network

TARGET = quadcopter
TEMPLATE = app

#LIBS += -lGLU

SOURCES += main.cpp\
        mainwindow.cpp \
    globject/vector.cpp \
    globject/material.cpp \
    globject/glquadcopter.cpp \
    globject/globject.cpp \
    globject/glgrid.cpp \
    globject/glcube.cpp \
    globject/glaxes.cpp \
    quaternion.cpp \
    globject/gltrace.cpp \
    globject/gltext.cpp \
    globject/glaxeswall.cpp \
    mymath.cpp \
    globject/glarrow.cpp \
    dialogcalibration/widgetview.cpp \
    dialogcalibration/matrix.cpp \
    dialogcalibration/fitting.cpp \
    dialogcalibration/dialogcalibration.cpp \
    globject/glpoints.cpp \
    dialogscene/dialogscene.cpp \
    dialogscene/glwidget.cpp \
    dialogscene/glview.cpp \
    dialogscene/autoscope.cpp \
    dialogdatasource/dialogdatasource.cpp \
    dialogdatasource/textfileparser.cpp \
    dialogdatasource/binaryparser.cpp \
    dialogdatasource/qextserialport/win_qextserialport.cpp \
    dialogdatasource/qextserialport/qextserialport.cpp \
    dialogdatasource/qextserialport/qextserialbase.cpp \
    dialogdatasource/qextserialport/posix_qextserialport.cpp \
    dialogstatus/dialogstatus.cpp \
    dialogcontrol/dialogcontrol.cpp \
    dialogcontrol/controllockattitudewidget.cpp \
    dialogmessage/dialogmessage.cpp \
    dialogbootloader/dialogbootloader.cpp \
    dialogbootloader/filereader.cpp \
    dialogparameter/dialogparameter.cpp \
    dialogcontrol/dialogcameracontrol.cpp

HEADERS  += mainwindow.h \
    globject/vector.h \
    globject/material.h \
    globject/glquadcopter.h \
    globject/globject.h \
    globject/glgrid.h \
    globject/glcube.h \
    globject/glaxes.h \
    quaternion.h \
    protocol.h \
    globject/gltrace.h \
    globject/gltext.h \
    globject/glaxeswall.h \
    mymath.h \
    globject/glarrow.h \
    dialogcalibration/widgetview.h \
    dialogcalibration/matrix.h \
    dialogcalibration/fitting.h \
    dialogcalibration/dialogcalibration.h \
    globject/glpoints.h \
    dialogscene/dialogscene.h \
    dialogscene/glwidget.h \
    dialogscene/glview.h \
    dialogscene/autoscope.h \
    dialogdatasource/dialogdatasource.h \
    dialogdatasource/textfileparser.h \
    dialogdatasource/binaryparser.h \
    dialogdatasource/qextserialport/win_qextserialport.h \
    dialogdatasource/qextserialport/qextserialport.h \
    dialogdatasource/qextserialport/qextserialbase.h \
    dialogdatasource/qextserialport/posix_qextserialport.h \
    dialogstatus/dialogstatus.h \
    dialogcontrol/dialogcontrol.h \
    dialogcontrol/controllockattitudewidget.h \
    dialogmessage/dialogmessage.h \
    dialogbootloader/dialogbootloader.h \
    dialogbootloader/filereader.h \
    dialogparameter/dialogparameter.h \
    dialogcontrol/dialogcameracontrol.h

FORMS    += mainwindow.ui \
    dialogcalibration/dialogcalibration.ui \
    dialogscene/dialogscene.ui \
    dialogdatasource/dialogdatasource.ui \
    dialogstatus/dialogstatus.ui \
    dialogcontrol/dialogcontrol.ui \
    dialogmessage/dialogmessage.ui \
    dialogbootloader/dialogbootloader.ui \
    dialogparameter/dialogparameter.ui \
    dialogcontrol/dialogcameracontrol.ui

OTHER_FILES += \
    protocol.txt
