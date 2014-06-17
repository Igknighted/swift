#include "lib/swift.cpp"

int main(){
	swift * render = new swift("text/html");

	// if the form is submitted, we will spit out the data from it
	if(render->requestlen("formdata") > 0)
	{
		render->gettemplate("html/example3_2.tpl.html");
		
		// get the data from our form after it is submitted
		string firstname = render->request("firstname");
		string lastname = render->request("lastname");
		
		// assign it to the templates so we can use it
		render->assign("firstname", firstname);
		render->assign("lastname", lastname);
		
		render->parse("main");
		
		render->display();
	// if the form is not submitted, we will spit out a form to fill out.
	}else{
		render->gettemplate("html/example3_1.tpl.html");
		
		render->parse("main");
		
		render->display();
	}
	
	return 0;
}
