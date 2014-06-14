#include "lib/swift.cpp"

int main(){
	swift * render = new swift("text/html");

	// This is our HTML template file that we are going to parse through.
	render->gettemplate("html/example2.tpl.html");

	// the template is parsed out in blocks for reusability.
	render->parse("main");
	
	// spit out the content of myblock1 with different variable assignments
	render->assign("foovar", "This is foovar...");
	render->parse("main.myblock1");
	
	render->assign("foovar", "We have now changed foovar");
	render->parse("main.myblock1");
	
	
	// this states that we are done parsing through our output, we spit it out now
	render->display();

	return 0;
}
