// !buildon - This lest swiftbuilder that we can use the buildcmd to make this
// This option is read by swiftbuilder to set the the application build command on edit
// !buildcmd: g++ -ldl -export-dynamic -march=native __filename__ -o __name__.cgi

#include "lib/swift.cpp"

int main(){
	swift * render = new swift("text/html");

	// This is our HTML template file that we are going to parse through.
	render->gettemplate("html/example1.tpl.html");

	// the template is parsed out in blocks for reusability.
	render->parse("main");
	
	// spit out the content of myblock1 two times
	render->parse("main.myblock1");
	render->parse("main.myblock1");
	render->parse("main.myblock1.test");
	
	
	// this states that we are done parsing through our output, we spit it out now
	render->display();

	return 0;
}
