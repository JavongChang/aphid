SOURCES = main.cpp RegexUi.cpp
HEADERS = RegexUi.h
INCLUDEPATH += /Users/jianzhang/Library/boost_1_44_0
QMAKE_LIBDIR += /Users/jianzhang/Library/boost_1_44_0/stage/lib
LIBS += -lboost_filesystem\
            -lboost_system\
            -lboost_regex
