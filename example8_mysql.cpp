// !buildon - This lest swiftbuilder that we can use the buildcmd to make this
// This option is read by swiftbuilder to set the the application build command on edit
// !buildcmd: g++ -ldl -export-dynamic  `/usr/bin/mysql_config --cflags` `/usr/bin/mysql_config --libs` __filename__ -o __name__.cgi
//
// Note, you need to install the devel package for your MySQL version. Examples:
// # yum install mysql56u-devel
// # yum install mysql55-devel
// # yum install mysql-devel
//
// Identify the package naming convention with:
// # rpm -qa | grep mysql | grep common
//
// Found out about this here:
// http://www.cyberciti.biz/tips/linux-unix-connect-mysql-c-api-program.html


#include "lib/swift.cpp"

#include <mysql.h>

swift * render;

int main(){

	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char *server = "localhost";
	char *user = "root";
	char *password = "PASSWORD"; /* set me first */
	char *database = "mysql";
	conn = mysql_init(NULL);

	render = new swift("text/html");


	/* Connect to database */
	if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
	/* send SQL query */
	if (mysql_query(conn, "show tables")) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}

	res = mysql_use_result(conn);
	
	/* output table name */
	printf("MySQL Tables in mysql database:\n");
	
	while ((row = mysql_fetch_row(res)) != NULL)
		printf("%s \n", row[0]);
	
	/* close connection */
	mysql_free_result(res);
	mysql_close(conn);
	
	render->display(); // this is when the header gets sent

	// we're just going to use a hooker here:
	render->hook("test", "./hooks/");
	
	return 0;
}


