-- Sostituire poi con gli utenti veri
CREATE USER 'test-user'@'localhost' IDENTIFIED BY 'testtest';
GRANT ALL PRIVILEGES ON * . * TO 'test-user'@'localhost';
FLUSH PRIVILEGES;


DROP SCHEMA IF EXISTS `gestione_lingue_straniere`;
CREATE SCHEMA `gestione_lingue_straniere`;
USE `gestione_lingue_straniere`; 

CREATE TABLE `Livelli`
(
	denominazione VARCHAR(30) PRIMARY KEY,
    titolo_libro VARCHAR(30),
    esame_necessario BOOLEAN
);

CREATE TABLE `Corsi`
(
	codice INTEGER PRIMARY KEY AUTO_INCREMENT, 
    livello VARCHAR(30),
    data_creazione DATE,
    FOREIGN KEY(livello) REFERENCES Livelli(denominazione)
);

CREATE TABLE `Lezioni`
(
	aula VARCHAR(4),
    giorno DATE,
    ora TIME,
    corso INTEGER,
    PRIMARY KEY (aula, giorno, ora),
    FOREIGN KEY (corso) REFERENCES Corsi(codice)
);

CREATE TABLE `Insegnanti`
(
	cf VARCHAR(16) PRIMARY KEY,
    pwd VARCHAR(32),
    cognome VARCHAR(30),
    nome VARCHAR(30),
    indirizzo VARCHAR(60),
    nazione VARCHAR(15)
);

CREATE TABLE `Allievi`
(
	cf VARCHAR(16) PRIMARY KEY,
    pwd VARCHAR(32),
    cognome VARCHAR(30),
    nome VARCHAR(30),
    telefono VARCHAR(10),
    corso INTEGER,
    data_iscrizione DATE,
    FOREIGN KEY(corso) REFERENCES Corsi(codice)
	
);

CREATE TABLE `Lezioni_Private`
(
    allievo VARCHAR(16),
    giorno DATE,
    ora TIME,
    insegnante VARCHAR(16),

    PRIMARY KEY (allievo, giorno, ora),
    FOREIGN KEY (insegnante) REFERENCES Insegnanti(cf),
    FOREIGN KEY (allievo) REFERENCES Allievi(cf)
);

CREATE TABLE `Docenze`
(
	insegnante VARCHAR(16),
    corso INTEGER,
    PRIMARY KEY (insegnante, corso),
    FOREIGN KEY (insegnante) REFERENCES Insegnanti(cf),
    FOREIGN KEY (corso) REFERENCES Corsi(codice)
);

CREATE TABLE `Assenze`
(
	allievo VARCHAR(16),
    giorno DATE,
    ora TIME,
    aula VARCHAR(4),

	PRIMARY KEY (allievo, aula, giorno, ora),
    FOREIGN KEY (allievo) REFERENCES Allievi(cf),
    FOREIGN KEY (aula, giorno, ora) REFERENCES Lezioni(aula, giorno, ora)
);

CREATE TABLE `Film`
(
    titolo VARCHAR(60),
    cognome_regista VARCHAR(30),
    nome_regista VARCHAR(30),
    PRIMARY KEY(titolo, cognome_regista)
);

CREATE TABLE `Argomenti_Conferenze`
(
    argomento VARCHAR(60),
    cognome_conferenziere VARCHAR(30),
    nome_conferenziere VARCHAR(30),
    PRIMARY KEY(argomento, cognome_conferenziere)
);

CREATE TABLE `Proiezioni`
(
	codice INTEGER PRIMARY KEY AUTO_INCREMENT,
    giorno DATE,
    ora TIME,
    film VARCHAR(60),
    cognome_regista VARCHAR(30),
    FOREIGN KEY(film, cognome_regista) REFERENCES Film(titolo, cognome_regista)
);

CREATE TABLE `Conferenze`
(
	codice INTEGER PRIMARY KEY AUTO_INCREMENT,
    giorno DATE,
    ora TIME,
    argomento VARCHAR(60),
    cognome_conferenziere VARCHAR(30),
    FOREIGN KEY(argomento, cognome_conferenziere) REFERENCES Argomenti_Conferenze(argomento, cognome_conferenziere)
);

CREATE TABLE `Prenotazioni_Proiezioni`
(
	allievo VARCHAR(16),
    codice_proiezione INTEGER,
    PRIMARY KEY (allievo, codice_proiezione),
    FOREIGN KEY (allievo) REFERENCES Allievi(cf),
    FOREIGN KEY (codice_proiezione) REFERENCES Proiezioni(codice)
);

CREATE TABLE `Prenotazioni_Conferenze`
(
	allievo VARCHAR(16),
    codice_conferenza INTEGER,
    PRIMARY KEY (allievo, codice_conferenza),
    FOREIGN KEY (allievo) REFERENCES Allievi(cf),
    FOREIGN KEY (codice_conferenza) REFERENCES Conferenze(codice)
);

-- Viste 

-- Verra usata per i report, sia dalla segreteria sia dai docenti 
DROP VIEW IF EXISTS Impegni_Insegnante;
CREATE VIEW Impegni_Insegnante 
AS
(SELECT i.cf AS CF_Insegnante, i.cognome AS Cognome, i.nome AS Nome, lp.giorno 
	AS Giorno, lp.ora AS Ora, "--" AS Aula, 'Lezione Privata' AS Tipologia
FROM Insegnanti i JOIN Lezioni_Private lp ON i.cf = lp.insegnante)
UNION
(SELECT i.cf AS CF_Insegnante, i.cognome AS Cognome, i.nome AS Nome, l.giorno 
	AS Giorno, l.ora AS Ora, l.aula AS Aula, 'Lezione Corso' AS Tipologia
FROM Insegnanti i JOIN Docenze d ON i.cf = d.insegnante 
			   JOIN Corsi c ON d.corso = c.codice 
			   JOIN Lezioni l ON l.corso = c.codice
) 
ORDER BY Cognome, Nome, Giorno, Ora;

-- Verra usata dagli studenti per prenotare una lezione privata
DROP VIEW IF EXISTS Prenotazioni_Lezioni_Private;
CREATE VIEW Prenotazioni_Lezioni_Private
AS
SELECT a.cf AS CF_Allievo, a.cognome AS Cognome_Allievo, a.nome AS Nome_Allievo, 
	i.cognome AS Cognome_Insegnante, i.nome AS Nome_Insegnante, lp.giorno 
	AS Giorno, lp.ora AS Ora
FROM Allievi a JOIN Lezioni_Private  lp ON lp.allievo = a.cf
	JOIN Insegnanti i ON lp.insegnante = i.cf;

-- Verra usata dagli studenti per consultare le proprie prenotazioni
DROP VIEW IF EXISTS PrenotazioniAttive;
CREATE VIEW PrenotazioniAttive
AS
(SELECT pren.allievo AS CF_studente, 'Proiezione' AS Tipologia, pro.giorno AS Giorno, pro.ora AS Ora, pro.film AS TitoloArgomento, pro.cognome_regista AS CognomeAutore, f.nome_regista AS NomeAutore
FROM Prenotazioni_Proiezioni pren JOIN Proiezioni pro ON pren.codice_proiezione = pro.codice 
    JOIN Film f ON (pro.film = f.titolo AND pro.cognome_regista = f.cognome_regista))
UNION
(SELECT pren.allievo AS CF_studente, 'Conferenza' AS Tipologia, c.giorno AS Giorno, c.ora 
	AS Ora, c.argomento AS TitoloArgomento, c.cognome_conferenziere 
    AS CognomeAutore, a.nome_conferenziere AS NomeAutore
FROM Prenotazioni_Conferenze pren JOIN Conferenze c ON pren.codice_conferenza = c.codice 
    JOIN Argomenti_Conferenze a ON (c.argomento = a.argomento AND c.cognome_conferenziere = a.cognome_conferenziere))
ORDER BY Giorno, Ora, TitoloArgomento;


-- Le seguenti viste ricongiungono Proiezioni/Film e Conferenze/Argomenti_Conferenze

DROP VIEW IF EXISTS ProiezioniCompleta;
CREATE VIEW ProiezioniConFilm
AS
(
	SELECT  p.codice AS codice, p.giorno AS giorno, p.ora AS ora, p.film AS film, p.cognome_regista AS cognome_regista, f.nome_regista AS nome_regista
	FROM Proiezioni p JOIN Film f ON p.film = f.titolo AND p.cognome_regista = f.nome_regista 
);


DROP VIEW IF EXISTS ConferenzeCompleta;
CREATE VIEW ConferenzeCompleta
AS
(
	SELECT  c.codice AS codice, c.giorno AS giorno, c.ora AS ora, c.argomento AS argomento, c.cognome_conferenziere AS cognome_conferenziere, ac.nome_conferenziere AS nome_conferenziere
	FROM Conferenze c JOIN Argomenti_Conferenze ac ON c.argomento = c.argomento AND c.cognome_conferenziere = ac.cognome_conferenziere
);




DELIMITER $$
CREATE PROCEDURE ATTIVA_CORSO(IN lvl VARCHAR(30))
BEGIN
	START TRANSACTION;

	INSERT INTO Corsi(Livello)
	VALUES(lvl);

	COMMIT;
END $$

DELIMITER ;


DELIMITER $$
CREATE PROCEDURE Registra_Allievo(IN cf_in VARCHAR(16), IN pwd_in VARCHAR(32), IN cognome_in VARCHAR(30), IN nome_in VARCHAR(30), IN tel_in VARCHAR(10), IN corso_in INTEGER)
BEGIN
	START TRANSACTION;

	INSERT INTO Allievi(cf, pwd, cognome, nome, telefono, corso, data_iscrizione)
	VALUES(cf_in, MD5(pwd_in), cognome_in, nome_in, tel_in, corso_in, CURDATE());

	COMMIT;
END $$

DELIMITER ;

DELIMITER $$
CREATE PROCEDURE ASSEGNA_INSEGNANTE(IN insegnante_in VARCHAR(10), IN corso_in INTEGER)
BEGIN
	START TRANSACTION;

	INSERT INTO Docenze(insegnante, corso)
	VALUES(insegnante_in, corso_in);

	COMMIT;
END $$


CREATE PROCEDURE PRENOTA_LEZIONE(IN	insegnante_in VARCHAR(16), IN allievo_in VARCHAR(16), IN giorno_in DATE, IN ora_in TIME)
BEGIN
	START TRANSACTION;

	INSERT INTO Lezioni_Private(insegnante, allievo, giorno, ora)
	VALUES(insegnante_in, allievo_in, giorno_in, ora_in);

	COMMIT;
END $$


CREATE PROCEDURE Nuova_Attivita(IN TIPOLOGIA INTEGER, IN giorno_in DATE, IN ora_in TIME, IN argfilm_in VARCHAR(60), IN cognome_in VARCHAR(30), IN nome_in VARCHAR(30))
BEGIN
	-- Se è una proiezione
	IF TIPOLOGIA = 0 THEN
		-- Se la proiezione si riferisce ad un nuovo film ne inserisco uno nuovo
		IF nome_in IS NOT NULL THEN
			INSERT INTO Film(titolo, cognome_regista, nome_regista)
			VALUES (argfilm_in, cognome_in, nome_in);
		END IF;
		    INSERT INTO Proiezioni(giorno, ora, film, cognome_regista)
			VALUES(giorno_in, ora_in, argfilm_in, cognome_in);

	-- Se è una conferenza
	ELSEIF TIPOLOGIA = 1 THEN

		IF nome_in IS NOT NULL THEN
			INSERT INTO Argomenti_Conferenze(argomento, cognome_conferenziere, nome_conferenziere)
			VALUES (argfilm_in, cognome_in, nome_in);
		END IF;
		    INSERT INTO Conferenze(giorno, ora, argomento, cognome_conferenziere)
			VALUES(giorno_in, ora_in, argfilm_in, cognome_in);

	ELSE
		ROLLBACK;
	END IF;

	COMMIT;
END $$

CREATE PROCEDURE PRENOTAZIONE_ATTIVITA(IN TIPOLOGIA INTEGER, IN allievo_in VARCHAR(16), IN codice_in INTEGER)
BEGIN
	-- Se è una proiezione
	IF TIPOLOGIA = 0 THEN
		INSERT INTO Prenotazioni_Proiezioni(allievo, codice_proiezione)
		VALUES (allievo_in, codice_in);

	-- Se è una conferenza
	ELSEIF TIPOLOGIA = 1 THEN
		INSERT INTO Prenotazioni_Conferenze(allievo, codice_conferenza)
		VALUES (allievo_in, codice_in);

	ELSE
		ROLLBACK;
	END IF;

	COMMIT;
END $$


CREATE PROCEDURE Nuovo_Anno()
BEGIN
	
	TRUNCATE TABLE Iscrizioni;
	TRUNCATE TABLE Docenze;
	TRUNCATE TABLE Lezioni;
	TRUNCATE TABLE Assenze;
	TRUNCATE TABLE Prenotazioni_Conferenze;
	TRUNCATE TABLE Prenotazioni_Proiezioni;
	TRUNCATE TABLE Conferenze;
	TRUNCATE TABLE Proiezioni;
	TRUNCATE TABLE Corsi;

	COMMIT;
END $$



INSERT INTO Livelli(denominazione, titolo_libro, esame_necessario)
VALUES('Intermediate', 'Cambridge Book', TRUE);

INSERT INTO Insegnanti(cf, pwd, cognome, nome, indirizzo, nazione)
VALUES('AAAAAAAAAAAAAAAA', MD5('password'), 'Francesco', 'Rossi', 'Via Roma 25', 'UK');

CALL `gestione_lingue_straniere`.`ATTIVA_CORSO`('Intermediate');
CALL `gestione_lingue_straniere`.`REGISTRA_ALLIEVO`('BBBBBBBBBBBBBBBB', 'pwd', 'Bianchi', 'Valentino', '0660600600', 1);

INSERT INTO Lezioni(aula, giorno, ora, corso) VALUES ("B2", CURDATE(), NOW(), 1);

INSERT INTO Corsi(Livello) VALUES('Intermediate');

INSERT INTO Docenze(insegnante, corso) VALUES ('AAAAAAAAAAAAAAAA', 1);

CALL `gestione_lingue_straniere`.`Nuova_Attivita`(0, '2019-4-4', '7:00:00', 'gOODByeLenIn', 'cAPATONDA', 'MACCIO');

CALL `gestione_lingue_straniere`.`Nuova_Attivita`(1, '2019-4-4', '7:00:00', 'gOODByeLenInMaIspROIEZIONE', 'cAPATONDA', 'MACCIO');



COMMIT;
