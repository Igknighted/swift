#include "lib/swift.cpp"

int main(){
	swift * render = new swift("text/html");

	// you can also set headers with swift. This is just a 403 redirect to google
	// the first parameter is just an arbitrary identifier in case you want to remove it
	// the second identifier is the actual custom header
	render->header("redirect", "Location: http://www.google.com/");
	
	render->display(); // this is when the header gets sent
	
	return 0;
}
