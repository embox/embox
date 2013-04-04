SOURCES += \
    main.cpp \
    mainwindow.cpp \
    openfiledialog.cpp \
    createfiledialog.cpp \
    savefiledialog.cpp \
    helpwindow.cpp

HEADERS += \
    mainwindow.h \
    openfiledialog.h \
    createfiledialog.h \
    savefiledialog.h \
    helpwindow.h

RESOURCES = texteditor.qrc

sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS texteditor.pro
