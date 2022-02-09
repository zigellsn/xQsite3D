#include "xQsite3D.h"
#include "MainEngine.h"

using namespace std;
//using namespace boost::python;

int main(int argc, char **argv) {
    int w = 1024, h = 768;
    string scene = "res/quad.dae";
    bool fullscreen = false;

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            stringstream ss;
            ss << argv[i];
            string arg;
            ss >> arg;
            if (arg[0] == '-' && arg[1] == 'w') {
                istringstream(arg.substr(2, arg.size() - 2)) >> w;
            }
            if (arg[0] == '-' && arg[1] == 'h') {
                istringstream(arg.substr(2, arg.size() - 2)) >> h;
            }
            if (arg[0] == '-' && arg[1] == 'f')
                fullscreen = true;
            if (arg[0] == '-' && arg[1] == 's')
                scene = arg.substr(2, arg.size() - 2);
        }
    }

//    try {
//        Py_Initialize();
//    }
//    catch(error_already_set const &) {
//        PyErr_Print();
//    }

    auto *ogl = new MainEngine(w, h, fullscreen);
    ogl->ImportScene(scene);
    ogl->MainLoop();
    delete ogl;
    return 0;
}
