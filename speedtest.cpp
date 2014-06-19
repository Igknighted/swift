// !buildon - This lest swiftbuilder that we can use the buildcmd to make this
// This option is read by swiftbuilder to set the the application build command on edit
// !buildcmd: g++ -O3 -ldl -export-dynamic __filename__ -o __name__.cgi

#include "lib/swift.cpp"

int main(){
	swift * render = new swift("text/html");

	// this states that we are done parsing through our output, we spit it out now
	render->display();

	cout << "Hello World" << endl;

	for(int n=0;n<=10000;n++)
		cout << n;

	return 0;
}
