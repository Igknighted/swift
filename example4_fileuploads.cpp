#include "lib/swift.cpp"

int main(){
	swift * render = new swift("text/html");

	// if the form is submitted, we will spit out the data from it
	if(render->requestlen("filedata") > 0)
	{
		render->gettemplate("html/example4_2.tpl.html");
		
		// it will take the request data for "filedata" from your form and store it as "savedfile.jpg"
		render->requeststore("filedata", "savedfile.jpg");
		
		// our template just references to the uploaded file location
		render->parse("main");
		
		render->display();
	// if the form is not submitted, we will spit out a form to fill out.
	}else{
		render->gettemplate("html/example4_1.tpl.html");
		
		render->parse("main");
		
		render->display();
	}
	
	return 0;
}
