// !buildon - This lest swiftbuilder that we can use the buildcmd to make this
// This option is read by swiftbuilder to set the the application build command on edit
// !buildcmd: g++ -ldl -export-dynamic __filename__ -o __name__.cgi

#include "lib/swift.cpp"

swift * render;

string testvar;

int main(){
	testvar = "This is a proper hooker!";

	render = new swift("text/html");

	
	render->display(); // this is when the header gets sent

	// we're just going to use a hooker here:
	render->hook("test", "./hooks/");
	
	return 0;
}
