# Directory di esempio per la consegna e la prova del progetto

Per consegnare il sorgente del progetto (che verrà utilizzata per la discussione) è sufficiente organizzare una cartella (chiamata con il numero della propria matricola) come nel presente archivio.

La cartella dovrà necessariamente contenere due file:

- `Makefile`: il makefile dovrà generare il programma che implementa il thin client in C. Il programma deve essere chiamato `applicazione`. Il `Makefile` di esempio genera l'applicazione in questo modo
- `db.sql`: questo script deve inizializzare la base di dati, eventualmente anche con i dati preinizializzati per consentire l'utilizzo dell'applicazione
- `config.json`: un file di configurazione per connettersi alla base di dati generata da `db.sql`.

**Attenzione**: `db.sql` deve anche creare gli utenti che sono necessari all'applicazione per funzionare.


## Istruzioni per testare l'applicazione

In sede d'esame, il codice consegnato verrà compilato ed eseguito all'interno di un ambiente Docker. Questo archivio contiene il `Dockerfile` e gli script necessari a generare un ambiente di test identico a quello che verrà utilizzato in fase di discussione del progetto.

Per generare l'immagine Docker (analoga a quella che verrà utilizzata in fase di discussione) si può utilizzare il comando:

    docker build -t bdc .

trovandosi nella cartella in cui è contenuto questo file README.

L'applicazione può essere testata lanciando il comando:

    docker run -e "STUDENTE=<matricola>" -it bdc

sostituendo a `<matricola>` la propria matricola (corrispondente al nome della sottocartella creata). Ad esempio, per testare questo archivio con la cartella di esempio, si può lanciare il comando:

    docker run -e "STUDENTE=1085431" -it bdc


## Attenzione!

La configurazione del Dockerfile è estremamente minimalista. Nell'improbabile caso che abbiate bisogno di software addizionale per far funzionare la vostra applicazione, indicatelo chiaramente nell'email che invierete al docente all'atto della consegna del progetto.

docker run -e "STUDENTE=0242178" -it bdc
