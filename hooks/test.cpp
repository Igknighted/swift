// Compile it:
// g++ -fPIC -shared -export-dynamic test.cpp -o test.so

#include <iostream>
using namespace std;

extern string testvar;

extern "C" {


	void test() {
		cout << "This is the test app <br>" << endl;
		cout << "Using a variable from the hooker parent: " << testvar << endl;
	}
}
