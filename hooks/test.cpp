// !buildon - This lest swiftbuilder that we can use the buildcmd to make this
// This option is read by swiftbuilder to set the the application build command on edit
// !buildcmd: g++ -fPIC -shared -export-dynamic __filename__ -o __name__.so

#include <iostream>
using namespace std;

extern string testvar;

extern "C" {


	void test() {
		cout << "This is the test app <br>" << endl;
		cout << "Using a variable from the hooker parent: " << testvar << endl;
	}
}
