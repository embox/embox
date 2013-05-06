SOURCES += \
    main.cpp \
    mainwindow.cpp \
    openfiledialog.cpp \
    createfiledialog.cpp \
    savefiledialog.cpp \
    helpwindow.cpp \
    login.cpp \
    desktopimage.cpp

HEADERS += \
    mainwindow.h \
    openfiledialog.h \
    createfiledialog.h \
    savefiledialog.h \
    helpwindow.h \
    login.h \
    desktopimage.h

RESOURCES = texteditor.qrc

sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS texteditor.pro
