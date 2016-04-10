#ifndef TEST_HELP_123
#define TEST_HELP_123

// qt
#include <QApplication>

/// defines a QApplication with dummy parameters for having message loop
// it's not a function since QApplication hasn't a move or copy constructor
#define CREATE_FAKE_APPLICATION_WITH_NO_ARGS(app) \
	char argv1_##app[] = "TESTAPP";\
	char* argv_##app[] ={&argv1_##app[0], nullptr};\
	int argc_##app = static_cast<int>(sizeof(argv_##app) / sizeof(char*) - 1);\
        QApplication app(argc_##app, &argv_##app[0])

#endif
