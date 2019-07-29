#include <stdio.h>


int main(int argc, char *argv[])
{
	
	int sceltaMenu = 0;

	printf("Gestionale Scuola Di Inglese");

mainMenu:
	printf("Selezionare l'utenza desiderata");
	printf("1 - Insegnante\n");
	printf("2 - Allievo\n");
	printf("3 - Segreteria\n");

	scanf("%d", sceltaMenu);
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

	printf("Inserisci il tuo codice fiscale\n");
	scanf("%s", cf);

	printf("Inserisci la password\n");
	scanf("%s", pwd);

	// Query 

	strcpy(curr_session, cf);
	printf("Benvenuto %s. Ecco la lista dei suoi impegni\n", curr_session);

	// Query impegni, fine


}


void gestioneStudente()
{
	char curr_session[11];
	int sceltaMenu = 0;
	char cf[11];
	char pwd[30];

	printf("Inserisci il tuo codice fiscale\n");
	scanf("%s", cf);

	printf("Inserisci la password\n");
	scanf("%s", pwd);

	// Query 

	strcpy(curr_session, cf);

menuStud:
	printf("Benvenuto %s. Scegli l'operazione\n", curr_session);
	printf("1 - Prenotazione ad evento\n");
	printf("2 - Prenotazione lezione privata\n");
	printf("3 - Lista lezioni private\n")

	scanf("%d", sceltaMenu);

	switch (sceltaMenu) {
		case 1:
			//
			break;

		case 2:
			//
			break;

		case 3:
			goto menuStud;
			break;


		}

}

