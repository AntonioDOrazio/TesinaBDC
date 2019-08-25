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


void menuPrincipale();
void gestioneInsegnante();
void gestioneAllievo();
void gestioneSegreteria();
void connessioneDB(char *nomeFile);
void printResult();

char q[512];
MYSQL *con;
MYSQL_RES *result;
MYSQL_ROW row;
MYSQL_FIELD *field;
int id, num_fields;
char curr_session[11];

int main(int argc, char *argv[])
{
	menuPrincipale();
	
	exit(EXIT_SUCCESS);
}


void gestioneInsegnante()
{
	int sceltaMenu = -1;
	char cf[17];
	char pwd[30];

	while(1)
	{
		printf("Inserisci il tuo codice fiscale\n");
		scanf("%s", cf);

		printf("Inserisci la password\n");
		scanf("%s", pwd);

		snprintf(q, 512, "SELECT cf FROM Insegnanti WHERE cf = '%s' AND pwd = MD5('%s')", cf, pwd);
		query(q);

		result = mysql_store_result(con);

		if (result == NULL || mysql_num_rows(result) == 0) {
			printf("Autenticazione fallita \n");
		} else { break; }
	}
	mysql_free_result(result);

	strcpy(curr_session, cf);

	while(1) 
 	{
		printf("Benvenuto %s. Scegli l'operazione\n", curr_session);
		printf("1 - Lista impegni\n");
		printf("2 - Segnala assenza\n");
		printf("0 - Menu principale\n");
		printf(">> ");
		scanf("%d", &sceltaMenu);

		switch (sceltaMenu) 
		{
			case 0:
				mysql_close(con);
				menuPrincipale();
				break;
			case 1:
				printf("\nEcco la lista dei suoi impegni\n");
				snprintf(q, 512, "SELECT CF_Insegnante,  Cognome,  Nome,  Giorno,  Ora,  Aula, Tipologia FROM Impegni_Insegnante WHERE CF_Insegnante = '%s' AND WEEK(Giorno, 1) = WEEK(NOW(),1)", cf);
				query(q);
				printResult();

				break;
			case 2:
				printf("\nInserimento assenza \n");

				int selezione = -1;
				char cf_allievo[17];
				int ora_assenza;
				char data_assenza[11];
				char aula_assenza[5];


				snprintf(q, 512, "SELECT c.codice, c.livello, c.data_attivazione FROM Corsi c JOIN Docenze d ON c.codice = d.corso WHERE d.insegnante = '%s'", cf);
				query(q);
				printResult();

				printf("Selezionare un corso >> ");
				scanf("%d", &selezione);
				

				snprintf(q, 512, "SELECT l.aula, l.giorno, l.ora FROM Lezioni l WHERE l.corso = %d AND giorno <= DATE(NOW())", selezione);
				query(q);

				printf("Elenco lezioni per corso fino ad oggi. \n");

				printResult();

				printf("CF Allievo >> ");
				scanf("%s", cf_allievo);
				printf("Ora lezione >> ");
				scanf("%d", &ora_assenza);
				printf("Data lezione (AAAA-MM-GG) >> ");
				scanf("%s", data_assenza);
				printf("Aula >> ");
				scanf("%s", aula_assenza);

				snprintf(q, 512, "CALL Aggiungi_Assenza('%s','%s','%d:00:00','%s', '%s')" , cf_allievo, data_assenza, ora_assenza, aula_assenza, cf);
				query(q);

				printf("Assenza registrata \n");

				break;
		}
	}

}

void gestioneAllievo()
{
	int sceltaMenu = -1;
	char cf[17];
	char pwd[30];

	while(1) 
	{
		printf("Inserisci il tuo codice fiscale\n");
		printf(">> ");
		scanf("%s", cf);

		printf("Inserisci la password\n");
		printf(">> ");
		scanf("%s", pwd);

		snprintf(q, 512, "SELECT cf FROM Allievi WHERE cf = '%s' AND pwd = MD5('%s')", cf, pwd);
		query(q);

		result = mysql_store_result(con);

		if (result == NULL || mysql_num_rows(result) == 0) {
			printf("Autenticazione fallita \n");		
		} else { break;	}
	}
	mysql_free_result(result);

	strcpy(curr_session, cf);

 	while(1) 
 	{
		printf("Benvenuto %s. Scegli l'operazione\n", curr_session);
		printf("1 - Iscrizione ad attivita\n");
		printf("2 - Prenotazione lezione privata\n");
		printf("3 - Lista lezioni private\n");
		printf("4 - Lista iscrizioni ad attivita\n");
		printf("5 - Lista assenze\n");

		printf("0 - Menu principale\n");
		printf(">> ");
		scanf("%d", &sceltaMenu);

		switch (sceltaMenu) 
		{
			case 0:
				mysql_close(con);
				menuPrincipale();
				break;
			case 1:
				printf("\nChe tipo di evento? \n");
				int tipologia = -1;

				while(tipologia != 0 && tipologia != 1)
				{
					printf("0 - Proiezione \n");
					printf("1 - Conferenza \n");
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

				printResult();

				int cod_att = -1;

				printf("Inserisci il codice dell'attivita desiderata >>  ");
				scanf("%d", &cod_att);

				snprintf(q, 512, "CALL Prenotazione_Attivita(%d, '%s', %d)", tipologia, cf, cod_att);
				query(q);

				printf("Prenotazione eseguita. \n"); 

				break;
			case 2:
				printf("\nElenco degli insegnanti \n");
				query("SELECT cf, cognome, nome FROM Insegnanti");
				printResult();

				char cf_insegnante[17];
				int ora_lezione;
				char data_lezione[11];

				printf("CF Insegnante >> ");
				scanf("%s", cf_insegnante);
				printf("Ora desiderata >> ");
				scanf("%d", &ora_lezione);
				printf("Data desiderata (AAAA-MM-GG) >> ");
				scanf("%s", data_lezione);


				snprintf(q, 512, "INSERT INTO Lezioni_Private(insegnante, allievo, giorno, ora) VALUES ('%s','%s','%s','%d:00:00')" , cf_insegnante, cf, data_lezione, ora_lezione);
				query(q);

				break;
			case 3:
				printf("\nLezioni private prenotate");
				snprintf(q, 512, "SELECT * FROM Prenotazioni_Lezioni_Private WHERE CF_Allievo = '%s'", cf);
				query(q);
				printResult();

				break;
			case 4:
				printf("\nProiezioni prenotate \n");
				
				snprintf(q, 512, "SELECT * FROM PrenotazioniAttive WHERE CF_allievo = '%s'", cf);

				query(q);

				printResult();		

				break;

			case 5:

				printf("\nLista assenze \n");

				snprintf(q, 512, "SELECT giorno, ora, aula FROM Assenze WHERE allievo = '%s'", cf);

				query(q);

				printResult(q);

				break; 

			default:
				printf("Scelta non valida\n");
				break;
			}
		}
}

void gestioneSegreteria()
{
	int sceltaMenu = -1;

	int id_corso;
	char cf_insegnante[17];

	char categoria[30];

	int ora_lezione;
	char data_lezione[11]; 
	char aula_lezione[5];

	char cf_allievo[17];
	char pwd_allievo[33];
	char cognome_allievo[31];
	char nome_allievo[31];
	char sesso_allievo[2];
	char data_nascita_allievo[11];
	char luogo_nascita_allievo[16];
	char indirizzo_allievo[61];
	int corso_allievo;

	char pwd_insegnante[33];
	char cognome_insegnante[31];
	char nome_insegnante[31];
	char indirizzo_insegnante[61];
	char nazione_insegnante[16];

	char titolo[60];
	char cognome_aut[30];
	char nome_aut[30];

	int ora_attivita;
	char data_attivita[11];


	while (1) {
		printf("Gestione segreteria. Scegli l'operazione \n");
		printf("1 - Attiva un nuovo corso\n");
		printf("2 - Aggiungi una lezione ad un corso\n");
		printf("3 - Iscrizione di un nuovo allievo\n");
		printf("4 - Registrazione di un nuovo insegnante\n");
		printf("5 - Assegnazione di un insegnante a un corso\n");
		printf("6 - Attivazione di una attivita culturale\n");
		printf("7 - Report mensile \n");
		printf("8 - Lista corsi attivi \n");
		printf("9 - Nuovo anno \n");
		printf("0 - Menu principale \n");
		printf(">> ");
		scanf("%d", &sceltaMenu);

		switch (sceltaMenu) {

		case 0:
			mysql_close(con);
			menuPrincipale();
			break;
		
		case 1:

			printf("Elenco livelli disponibili\n");

			snprintf(q, 512, "SELECT * FROM Livelli");
			query(q);

			printResult();	

			printf("\nInserisci il livello del corso da attivare\n");
			printf(">> ");
			scanf(" %31[^\n]", categoria);

			snprintf(q, 512, "INSERT INTO Corsi(livello, data_attivazione) VALUES ('%s', CURDATE())", categoria);
			query(q);

			printf("Corso di tipo %s attivato\n", categoria);

			break;

		case 2:

			printf("Elenco corsi attivi\n");

			snprintf(q, 512, "SELECT c.codice AS Codice, l.denominazione AS Denominazione, c.data_attivazione AS Data_Attivazione FROM Corsi c JOIN Livelli l ON c.livello = l.denominazione");
			query(q);

			printResult();

			printf("\nCodice corso >> ");
			scanf("%d", &id_corso);
			printf("Ora lezione >> ");
			scanf("%d", &ora_lezione);
			printf("Data lezione (AAAA-MM-GG) >> ");
			scanf("%s", data_lezione);
			printf("Aula >> ");
			scanf("%s", aula_lezione);


			snprintf(q, 512, "INSERT INTO Lezioni(aula, giorno, ora, corso) VALUES('%s', '%s', '%d:00:00', %d)", aula_lezione, data_lezione, ora_lezione, id_corso);
			query(q);

			printf("Lezione aggiunta \n");

			break;

		case 3:

			printf("\nCodice fiscale >> ");


			scanf("%s", cf_allievo);
			printf("Password >> ");
			scanf("%s", pwd_allievo);
			printf("Cognome >> ");
			scanf("%s", cognome_allievo);
			printf("Nome >> ");
			scanf("%s", nome_allievo);
			printf("Sesso (M/F) >> ");
			scanf("%s", sesso_allievo);
			printf("Data di nascita (AAAA-MM-GG) >> ");
			scanf("%s", data_nascita_allievo);
			printf("Luogo di nascita >> ");
			scanf("%s", luogo_nascita_allievo);
			printf("Indirizzo >> ");
			scanf(" %61[^\n]", indirizzo_allievo);
			printf("Id del corso >> ");
			scanf("%d", &corso_allievo);
			
			snprintf(q, 512, "INSERT INTO Allievi(cf, pwd, cognome, nome, sesso, data_nascita, luogo_nascita, indirizzo, corso, data_iscrizione) VALUES('%s', MD5('%s'), '%s', '%s', UPPER('%s'), '%s','%s','%s', %d, CURDATE())", cf_allievo, pwd_allievo, cognome_allievo, nome_allievo, sesso_allievo, data_nascita_allievo, luogo_nascita_allievo, indirizzo_allievo, corso_allievo);
			query(q);

			printf("Allievo %s registrato \n", cf_allievo);

			break;

		case 4: 

			printf("\nCodice fiscale >> ");

			scanf("%s", cf_insegnante);
			printf("Password >> ");
			scanf("%s", pwd_insegnante);
			printf("Cognome >> ");
			scanf("%s", cognome_insegnante);
			printf("Nome >> ");
			scanf("%s", nome_insegnante);
			printf("Sesso (M/F) >> ");
			scanf("%s", indirizzo_insegnante);
			printf("Nazione di provenienza >> ");
			scanf("%s", nazione_insegnante);

			
			snprintf(q, 512, "INSERT INTO Insegnanti(cf, pwd, cognome, nome, indirizzo, nazione) VALUES('%s', MD5('%s'), '%s', '%s', '%s','%s')", cf_insegnante, pwd_insegnante, cognome_insegnante, nome_insegnante, indirizzo_insegnante, nazione_insegnante);
			query(q);

			printf("Insegnante %s registrato \n", cf_insegnante);


			break;


		case 5:
			printf("\nElenco degli insegnanti \n");

			query("SELECT cf, cognome, nome, indirizzo, nazione FROM Insegnanti");
			
			printResult();

			printf("Elenco dei corsi attivi \n");

			query("SELECT * FROM Corsi");
			printResult();

			printf("CF dell'insegnante >> ");
			scanf("%s", cf_insegnante);
			printf("ID del corso >> ");
			scanf("%d", &id_corso);

			snprintf(q, 512, "INSERT INTO Docenze(insegnante, corso) VALUES ('%s', %d)", cf_insegnante, id_corso);
			query(q);

			printf("Insegnante assegnato \n");

			break;

		case 6:
			printf("\nAttivazione attivita culturale \n");

			int tipologia = -1;

			while(tipologia != 0 && tipologia != 1)
			{
				printf("Che tipo di evento? \n");
				printf("0 - Proiezione \n");
				printf("1 - Conferenza \n");
				printf(">> ");
				scanf("%d", &tipologia);
			}


			printf("Ora desiderata >> ");
			scanf("%d", &ora_attivita);
			printf("Data desiderata (AAAA-MM-GG) >> ");
			scanf("%s", data_attivita);


			printf("Titolo/argomento di film/conferenza >> ");
			scanf(" %61[^\n]", titolo);

			printf("Cognome regista/conferenziere >> ");
			scanf(" %31[^\n]", cognome_aut);

			printf("Nome regista/conferenziere >> ");
			scanf(" %31[^\n]", nome_aut);

			snprintf(q, 512, "CALL Nuova_Attivita('%d','%s','%d:00:00','%s','%s','%s');", tipologia, data_attivita, ora_attivita, titolo, cognome_aut, nome_aut);
			query(q);

			printf("Attivita creata. \n");

			break;

		case 7:
			printf("\nReport mensile \n");

			query("SELECT CF_Insegnante,  Cognome,  Nome,  Giorno,  Ora,  Aula, Tipologia FROM Impegni_Insegnante WHERE MONTH(Giorno) = MONTH(NOW()) ORDER BY CF_Insegnante, Giorno, Ora");
			
			printResult();

			break;

		case 8:
			printf("\nLista corsi attivi \n");

			snprintf(q, 512, "SELECT c.codice AS Codice, l.denominazione AS Livello, c.data_attivazione AS Data_Attivazione, COUNT(a.cf) AS Tot_Allievi FROM Corsi c JOIN Livelli l ON c.livello = l.denominazione JOIN Allievi a ON a.corso = c.codice GROUP BY c.codice");
			query(q);	

			printResult();

			break;		

		case 9:
			printf("## Reinizializzazione Anno ## \n");
			printf("ATTENZIONE: Saranno distrutti tutti i dati relativi all'anno scolastico memorizzato\n");
			char confirm[2];

			printf("Continuare? S/N \n");
			printf(">> ");
			fflush(stdin);
			scanf("%s", confirm);

			if (!strcmp(confirm, "s") || !strcmp(confirm, "S"))
			{
				query("CALL Nuovo_Anno()");
				printf("Anno reinizializzato. \n");
			} else { // Una qualunque altra risposta annulla l'operazione
				printf("Annullato. \n");
			}

			break;

		default:
			printf("Scelta non valida ");
			break;
		}
	}
}

void connessioneDB(char *nomeFile)
{
	con = mysql_init(NULL);

	load_file(&config, nomeFile);
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
}


void menuPrincipale()
{
	char configFile[32];
	int sceltaMenu = 0;

	printf("### Gestione di Corsi di Lingue Straniere ### \n");
	while(1)
	{
		printf("Selezionare l'utenza desiderata\n");
		printf("1 - Insegnante\n");
		printf("2 - Allievo\n");
		printf("3 - Segreteria\n");
		printf("0 - Termina il programma \n");
		printf(">> ");
		scanf("%d", &sceltaMenu);
		switch (sceltaMenu){
			case 1:
				strcpy(configFile, "config_insegnanti.json");
				connessioneDB(configFile);
				gestioneInsegnante();
				break;
			case 2:
				strcpy(configFile, "config_allievi.json");
				connessioneDB(configFile);
				gestioneAllievo();
				break;
			case 3:
				strcpy(configFile, "config_segreteria.json");
				connessioneDB(configFile);
				gestioneSegreteria();
				break;
			case 0:
				exit(EXIT_SUCCESS);
				break;
			default:
				printf("Scelta invalida. Riprovare \n");
				sceltaMenu = 0;
		}
	}
}

void printResult()
{
	printf("\n");

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

	printf("\n");

}