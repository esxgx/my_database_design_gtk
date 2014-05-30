#include <mysql/mysql.h>
#include <stdio.h>

#ifndef NULL
#define NULL	((void *)0)
#endif

static MYSQL mysql;

int db_init(char *host, char *user, char *pw, int port)
{
	if (mysql_init(&mysql) == &mysql) {
		if (mysql_real_connect(&mysql, host, user, pw, NULL, port, NULL, 0)) {
			mysql_query(&mysql, "CREATE DATABASE IF NOT EXISTS `qq`");
			mysql_query(&mysql, "USE qq");
			mysql_query(&mysql, "CREATE TABLE IF NOT EXISTS `Information` (`IPaddress` varchar(30),`Message` varchar(100))");
			return 0;
		}
		printf("Failed to initialize database: %s\n", mysql_error(&mysql));
	}
	return -1;
}

int db_addmsg(char *clientip, char *msg)
{
	char querystr[8192];
	sprintf(querystr, "INSERT INTO `Information`(`IPaddress`, `Message`) VALUES(\'%s\',\'%s\')",
		clientip, msg);
	if (mysql_query(&mysql, querystr)) {
		printf("[%s] Error %u: %s\n", __FUNCTION__, mysql_errno(&mysql), mysql_error(&mysql));
		return -1;
	}
	return 0;
}

int db_deleteall(void)
{
	if (mysql_query(&mysql, "DELETE FROM `Information`")) {
		printf("[%s] Error %u: %s\n", __FUNCTION__, mysql_errno(&mysql), mysql_error(&mysql));
		return -1;
	}
	return 0;
}

int db_querydata(void)
{
	if (mysql_query(&mysql, "SELECT * FROM `Information`")) {
		printf("[%s] Error %u: %s\n", __FUNCTION__, mysql_errno(&mysql), mysql_error(&mysql));
		return -1;
	}
	MYSQL_RES *result = mysql_store_result(&mysql);
	int num_fields = 0;	// mysql_num_fields(result);
	int ipaddress_col = 0, message_col = 1;
	MYSQL_FIELD *field;
	if (field = mysql_fetch_field(result)) {
		if (!strcmp(field->name, "IPaddress"))
			ipaddress_col = num_fields;
		else if (!strcmp(field->name, "Message"))
			message_col = num_fields;
		++num_fields;
	}

	MYSQL_ROW row;
	while (row = mysql_fetch_row(result))
		additem("from database", row[ipaddress_col], row[message_col]);
	mysql_free_result(result);
	return 0;
}

