#include "lib/swift.cpp"

int main(){
	swift * render = new swift("text/html");

	// if the form is submitted, we will spit out the data from it
	if(render->requestlen("userck") > 0)
	{
		render->gettemplate("html/example5.tpl.html");
		
		// set a cookie value for userck
		render->unsetcookie("userck");
		
		// our template just references to the uploaded file location
		render->parse("main");
		
		render->parse("main.cookieset");
		
		render->display();
	// if the form is not submitted, we will spit out a form to fill out.
	}else{
		render->gettemplate("html/example5.tpl.html");
		
		// set a cookie value for userck
		render->setcookie("userck", "beenhere");
		
		render->parse("main");
		
		render->parse("main.nocookieset");
		
		render->display();
	}
	
	return 0;
}
