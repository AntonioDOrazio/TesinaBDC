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


void gestioneInsegnante();
void gestioneAllievo();
void gestioneSegreteria();

char q[256];

MYSQL *con;
MYSQL_RES *result;
MYSQL_ROW row;
MYSQL_FIELD *field;
int id, num_fields;

int main(int argc, char *argv[])
{

	con = mysql_init(NULL);

	// manca create database 	

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


	char use_query[128] = "USE ";
	strncat(use_query, conf.database, 128);
	use_query[127] = '\0';
	if(mysql_query(con, use_query)) {
		finish_with_error(con, "Use");
	}

	int sceltaMenu = 0;

	printf("### Gestionale Scuola Di Inglese ### \n");

mainMenu:
	printf("Selezionare l'utenza desiderata\n");
	printf("1 - Insegnante\n");
	printf("2 - Allievo\n");
	printf("3 - Segreteria\n");
	printf(">> ");

	scanf("%d", &sceltaMenu);
	switch (sceltaMenu){
		case 1:
			gestioneInsegnante();
			break;
		case 2: 
			gestioneAllievo();
			break;
		case 3:
			gestioneSegreteria();
			break;
		default:
			printf("Scelta invalida. Riprovare \n");
			goto mainMenu;
	}
	
	return EXIT_SUCCESS;
}


void gestioneInsegnante()
{
	char curr_session[11];
	int sceltaMenu = 0;
	char cf[11];
	char pwd[30];

loginInsegnante:
	printf("Inserisci il tuo codice fiscale\n");
	scanf("%s", cf);

	printf("Inserisci la password\n");
	scanf("%s", pwd);

	// Query 
	snprintf(q, 256, "SELECT cf FROM Insegnanti WHERE cf = '%s' AND pwd = MD5('%s')", cf, pwd);
	query(q);

	result = mysql_store_result(con);
	if (result == NULL) {
		printf("Autenticazione fallita");
		goto loginInsegnante;
	}
	mysql_free_result(result);


	strcpy(curr_session, cf);
	printf("Benvenuto %s. Ecco la lista dei suoi impegni\n", curr_session);



	snprintf(q, 256, "SELECT CF_Insegnante,  Cognome,  Nome,  Giorno,  Ora,  Aula, Tipologia FROM Impegni_Insegnante WHERE CF_Insegnante = '%s'", cf);
	query("SELECT CF_Insegnante,  Cognome,  Nome,  Giorno,  Ora,  Aula, Tipologia FROM Impegni_Insegnante WHERE CF_Insegnante = 'AAAAAAAAAAAAAAAA'");
	
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


}


void gestioneAllievo()
{
	int sceltaMenu = -1;
	char cf[17];
	char pwd[30];

	printf("Inserisci il tuo codice fiscale\n");
	scanf("%s", cf);

	printf("Inserisci la password\n");
	scanf("%s", pwd);

	// Query 


 	while(sceltaMenu != 0) 
 	{
		printf("Benvenuto %s. Scegli l'operazione\n", cf);
		printf("1 - Iscrizione ad attivit�\n");
		printf("2 - Prenotazione lezione privata\n");
		printf("3 - Lista lezioni private\n");
		printf("4 - Lista iscrizioni ad attivit�\n");
		printf("0 - Termina il programma\n");

		scanf("%d", &sceltaMenu);

		switch (sceltaMenu) 
		{
			case 0:
				break;

			case 1:

				printf("Che tipo di evento? \n");

				int tipologia = -1;

				while(tipologia != 0 && tipologia != 1)
				{
					printf("0 - Proiezione \n");
					printf("1 - Attivita \n");
				
					scanf("%d", &tipologia);
				}

				if (tipologia == 0) 
				{

					query("SELECT * FROM ProiezioniCompleta WHERE giorno >= CURDATE()");

				}

				else if (tipologia == 1) 
				{

					query("SELECT * FROM ConferenzeCompleta WHERE giorno >= CURDATE()");

				}

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

				int cod_att = -1;

				printf("Inserisci il codice dell'attivita desiderata >>");
				scanf("%d", &cod_att);

				snprintf(q, 256, "CALL Prenotazione_Attivita(%d, '%s', %d,", tipologia, cf, cod_att);
				query(q);

				printf("Prenotazione eseguita. \n"); 

				break;

			case 2:

				printf("Elenco degli insegnanti \n");

				query("SELECT * FROM Insegnanti");
				
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

				char cf_insegnante[17];
				int ora;
				int giorno;
				int mese;
				int anno;
				char ora_format[9];
				char data_format[11];

				printf("CF Insegnante >> ");
				scanf("%s", cf_insegnante);
				printf("Ora desiderata >> ");
				scanf("%d", &ora);
				printf("Giorno desiderato >> ");
				scanf("%d", &giorno);
				printf("Mese desiderato (1-12) >> ");
				scanf("%d", &mese);
				printf("Anno desiderato >> ");
				scanf("%d", &anno);


				sprintf (ora_format, "%d:00:00", ora);
				sprintf (data_format, "%d-%d-%d", anno, mese, giorno);

				snprintf(q, 256, "INSERT INTO Lezioni_Private(insegnante, allievo, giorno, ora) VALUES ('%s','%s','%s','%s')" , cf_insegnante, cf, data_format, ora_format);
				query(q);


				break;

			case 3:

				snprintf(q, 256, "SELECT * FROM Lezioni_Private WHERE allievo = '%s'", cf);
				query(q);
					
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


			case 4:
				printf("Proiezioni prenotate \n");
				
				
				snprintf(q, 256, "SELECT  p.codice AS codice, p.giorno AS giorno, p.ora AS ora, p.film AS film, p.cognome_regista AS cognome_regista, p.nome_regista AS nome_regista AS FROM ProiezioniCompleta p JOIN Prenotazioni_Proiezioni pre ON p.codice = pre.codice_proiezione WHERE pre.allievo = '%s'", cf);
				query(q);

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


				printf("Conferenze prenotate \n");

				snprintf(q, 256, "SELECT  c.codice AS codice, c.giorno AS giorno, c.ora AS ora, c.argomento AS argomento, c.cognome_conferenziere AS cognome_conferenziere, c.nome_conferenziere AS nome_conferenziere FROM ConferenzeComleta c JOIN Prenotazioni_Conferenze pre ON p.codice = pre.codice_conferenza WHERE pre.allievo = '%s'", cf);

				query(q);

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


				break;

			default:
				printf("Scelta non valida ");
				break;


			}
		}
}

void gestioneSegreteria()
{
	int sceltaMenu = -1;


	while (sceltaMenu != 0) {
		printf("Gestione segreteria. Scegli l'operazione \n");
		printf("1 - Attiva un nuovo corso\n");
		printf("2 - Iscrizione di un nuovo allievo\n");
		printf("3 - Assegnazione di un insegnante a un corso\n");
		printf("4 - Attivazione di una attivit� culturale\n");
		printf("5 - Report mensile \n");
		printf("6 - Nuovo anno \n");
		printf("0 - Termina il programma \n");
		printf(">> ");
		scanf("%d", &sceltaMenu);

		switch (sceltaMenu) {

		case 0:
			break;

		case 1:

			printf("Inserisci il livello del corso da attivare\n");
			printf(">> ");
			char categoria[30];
			scanf("%s", categoria);

			snprintf(q, 256, "INSERT INTO Corsi(livello, data_creazione) VALUES ('%s', CURDATE())", categoria);
			query(q);

			printf("Corso di tipo %s attivato\n", categoria);

			break;

		case 2:

			printf("Codice fiscale >> ");

			char cf_allievo[17];
			char pwd_allievo[33];
			char cognome_allievo[31];
			char nome_allievo[31];
			char telefono_allievo[11];
			int corso_allievo;

			scanf("%s", cf_allievo);
			printf("Nuova password >> ");
			scanf("%s", pwd_allievo);
			printf("Cognome >> ");
			scanf("%s", cognome_allievo);
			printf("Nome >> ");
			scanf("%s", nome_allievo);
			printf("Telefono >> ");
			scanf("%s", telefono_allievo);
			printf("Id del corso >> ");
			scanf("%d", &corso_allievo);
			
			snprintf(q, 256, "INSERT INTO Allievi(cf, pwd, cognome, nome, telefono, corso, data_iscrizione) VALUES('%s', MD5('%s'), '%s', '%s', '%s', %d, CURDATE())", cf_allievo, pwd_allievo, cognome_allievo, nome_allievo, telefono_allievo, corso_allievo);
			query(q);

			printf("Alunno %s registrato \n", cf_allievo);


			break;

		case 3:
			printf("Elenco degli insegnanti \n");

			query("SELECT * FROM Insegnanti");
			
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

			printf("Elenco dei corsi attivi \n");

			query("SELECT * FROM Corsi");
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

			char cf_insegnante[17];
			int id_corso;

			printf("CF dell'insegnante >> ");
			scanf("%s", cf_insegnante);
			printf("ID del corso >> ");
			scanf("%d", &id_corso);

			snprintf(q, 256, "INSERT INTO Docenze(insegnante, corso) VALUES ('%s', %d)", cf_insegnante, id_corso);
			query(q);

			printf("Docente assegnato \n");

			break;

		case 4:

			printf("");

			int tipologia = -1;
			int svolto = -1;

			while(tipologia != 0 && tipologia != 1)
			{
				printf("Che tipo di evento? \n");
				printf("0 - Proiezione \n");
				printf("1 - Attivita \n");
			
				scanf("%d", &tipologia);
			}
			while(svolto != 0 && svolto != 1)
			{
				printf("Si tratta di un evento gia svolto in passato? \n");
				printf("0 - No \n");
				printf("1 - Si \n");
				scanf("%d", &svolto);

			}

			char titolo[60];
			char cognome_aut[30];
			char nome_aut[30];

			printf("Titolo/argomento di film/conferenza >> ");
			scanf("%s", titolo);

			printf("Cognome regista/conferenziere >> ");
			scanf("%s", cognome_aut);


			if (svolto == 0) 
			{
				printf("Nome regista/conferenziere >> ");
				scanf("%s", nome_aut);
			}
			else { strcpy(nome_aut, "NULL"); }

			int ora;
			int giorno;
			int mese;
			int anno;
			char ora_format[9];
			char data_format[11];

			printf("Ora desiderata >> ");
			scanf("%d", &ora);
			printf("Giorno desiderato >> ");
			scanf("%d", &giorno);
			printf("Mese desiderato (1-12) >> ");
			scanf("%d", &mese);
			printf("Anno desiderato >> ");
			scanf("%d", &anno);

			sprintf (ora_format, "%d:00:00", ora);
			sprintf (data_format, "%d-%d-%d", anno, mese, giorno);

			snprintf(q, 256, "CALL Nuova_Attivita('%d','%s','%s','%s','%s','%s');", tipologia, data_format, ora_format, titolo, cognome_aut, nome_aut);
			query(q);

			printf("Attivita creata. \n");
			
			

			break;

		case 5:
			//
			
			query("SELECT CF_Insegnante,  Cognome,  Nome,  Giorno,  Ora,  Aula, Tipologia FROM Impegni_Insegnante ORDER BY CF_Insegnante, Giorno, Ora");
			
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

			break;


		case 6:
			printf("## Reinizializzazione Anno ## \n");
			printf("ATTENZIONE: Saranno distrutti tutti i dati relativi all'anno scolastico memorizzato\n");
			if ( yesOrNo("Continuare?", 'Y', 'N', false, true) )
			{
				query("CALL Nuovo_Anno()");
				printf("Anno reinizializzato. \n");
			} else {
				printf("Annullato. \n");
			}

		default:
			printf("Scelta non valida ");
			break;

		}
		exit(EXIT_SUCCESS);

	}
}