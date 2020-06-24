#include "ofMain.h"
#include "ofApp.h"


//========================================================================
int main(int argc, char** argv ){
    cout << "argc=" << argc << endl;
    for(int i=0;i<argc;i++)
    {
        cout << i << ":" << argv[i] << endl;
    }
	ofSetupOpenGL(1280,768,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
