#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#include "program.h"

struct configuration conf;

#define query(Q) do { \
			if (mysql_query(con, Q)) { \
				finish_with_error(con, Q); \
			} \
		 } while(0)

static void finish_with_error(MYSQL *con, char *err)
{
	fprintf(stderr, "%s error: %s\n", err, mysql_error(con));
	mysql_close(con);
	exit(1);        
}

static void store_image(MYSQL *con, char *car_name) {
	char *image;
	char filename[128];
	size_t length, q_len;
	char *chunk, *query;
	char *basic_query = "INSERT INTO Images (`Car`, `Picture`)"
			    "VALUES ( "
				"(SELECT `Id` FROM Cars WHERE `Name` = '%s' LIMIT 1), "
				"'%s'"
			    ")";
	q_len = strlen(basic_query);    

	snprintf(filename, 128, "img/%s.jpg", car_name);
	length = load_file(&image, filename);

	// Get space to store the query
	chunk = malloc(2 * length + 1);
	query = malloc(q_len + 2 * length + 1);

	// Prevent SQL Injections
	mysql_real_escape_string(con, chunk, image, length);

	length = snprintf(query, q_len + 2 * length + 1, basic_query, car_name, chunk);

	// mysql_query() cannot be used when there are raw data in place
	// (Binary data may contain the \0 character, which mysql_query()
	// interprets as the end of the statement string.) 
	if (mysql_real_query(con, query, length)) {
		finish_with_error(con, "Image insertion");
	};

	free(chunk);
	free(query);
	free(image);
}

int main(int argc, char **argv)
{  
	MYSQL *con = mysql_init(NULL);
	MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_FIELD *field;
	
	int id, num_fields;
	char create_query[128] = "CREATE DATABASE IF NOT EXISTS ";
	char use_query[128] = "USE ";

	load_file(&config, "config.json");
	parse_config();
	dump_config();

	if(con == NULL) {
		fprintf(stderr, "Initilization error: %s\n", mysql_error(con));
		exit(1);
	}

	if(mysql_real_connect(con, conf.host, conf.username, conf.password, NULL, conf.port, NULL, 0) == NULL) {
		finish_with_error(con, "Connection");
	}

	puts("*** Creating the database ***");

	strncat(create_query, conf.database, 128);
	create_query[127] = '\0';
	if(mysql_query(con, create_query)) {
		finish_with_error(con, "Creation");
	}

	strncat(use_query, conf.database, 128);
	use_query[127] = '\0';
	if(mysql_query(con, use_query)) {
		finish_with_error(con, "Use");
	}

	// Create a fresh table
	puts("** Creating and Populating **");
	query("DROP TABLE IF EXISTS Cars");
	query("CREATE TABLE Cars ("
			"Id INT AUTO_INCREMENT,"
			"Name TEXT,"
			"Price INT,"
			"PRIMARY KEY (Id)"
	      ")");

	// Populate
	query("INSERT INTO Cars VALUES (1,'Audi',52642)");
	query("INSERT INTO Cars VALUES (2,'Mercedes',57127)");
	query("INSERT INTO Cars VALUES (3,'Skoda',9000)");
	query("INSERT INTO Cars VALUES (4,'Volvo',29000)");
	query("INSERT INTO Cars VALUES (5,'Bentley',350000)");
	query("INSERT INTO Cars VALUES (6,'Citroen',21000)");
	query("INSERT INTO Cars VALUES (7,'Hummer',41400)");
	query("INSERT INTO Cars VALUES (8,'Volkswagen',21600)");

	// Retrieve the ID of the last insertion
	id = mysql_insert_id(con); // Only works if there is an autoincrement key
	printf("Last inserted id: %d\n", id);

	// Retrieve the inserted data
	puts("**    Dumping the data    ***");
	query("SELECT * FROM Cars");

	result = mysql_store_result(con);
	if (result == NULL) {
		finish_with_error(con, "Select");
	}

	num_fields = mysql_num_fields(result);

	// Dump header on screen
	while(field = mysql_fetch_field(result)) {
		printf("%s ", field->name);
	}
	printf("\n");
	// Dump data on screen
	while ((row = mysql_fetch_row(result))) { 
		for(int i = 0; i < num_fields; i++) {
			printf("%s ", row[i] ? row[i] : "NULL");
		} 
		printf("\n"); 
	}
	mysql_free_result(result);

	// Create and populate image table
	query("DROP TABLE IF EXISTS Images");
	query("CREATE TABLE Images ("
			"Id INT AUTO_INCREMENT,"
			"Car INT,"
			"Picture MEDIUMBLOB,"
			"PRIMARY KEY (Id)"
	      ")");

	// Store images
	store_image(con, "Volkswagen");
	store_image(con, "Hummer");
	store_image(con, "Citroen");
	store_image(con, "Bentley");
	store_image(con, "Volvo");
	store_image(con, "Skoda");
	store_image(con, "Mercedes");
	store_image(con, "Audi");


	if(yesOrNo("Do you want to open an image?", 'Y', 'N', false, true)) {
		char nome[64];
		char q[128];
		printf("Insert the car name: ");
		getInput(128, nome, false);
		printf("Retrieving image for %s\n", nome);

		snprintf(q, 128, "SELECT I.Picture FROM Images I RIGHT JOIN Cars C ON C.Id = I.Car WHERE C.Name = '%s'", nome);
		query(q);

		result = mysql_store_result(con);
		if (result == NULL) {
			finish_with_error(con, "Car not found");
		}

		row = mysql_fetch_row(result);
		unsigned long *lengths = mysql_fetch_lengths(result);
  
		if (lengths == NULL) {
			finish_with_error(con, "Retrieving length");
		}

		FILE *fp = fopen(".tmp.jpg", "wb");
		if (fp == NULL) {
			fprintf(stderr, "cannot open temporary file\n");
			exit(1);
		}

		fwrite(row[0], lengths[0], 1, fp);

		system("xdg-open .tmp.jpg");

		//remove(".tmp.jpg");
	}

	mysql_close(con);
	return 0;
}
