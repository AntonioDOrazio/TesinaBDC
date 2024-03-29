CREATE USER 'user-insegnanti'@'localhost' IDENTIFIED BY 'insegnantipwd';
CREATE USER 'user-allievi'@'localhost' IDENTIFIED BY 'allievipwd';
CREATE USER 'user-segreteria'@'localhost' IDENTIFIED BY 'segreteriapwd';


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
    data_attivazione DATE,
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
    sesso ENUM('M', 'F'),
    data_nascita DATE,
    luogo_nascita VARCHAR(15),
    indirizzo VARCHAR(60),
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


CREATE TABLE `Registi`
(
	id_regista INTEGER PRIMARY KEY AUTO_INCREMENT,
    cognome_regista VARCHAR(30),
    nome_regista VARCHAR(30)
);


CREATE TABLE `Film`
(
	id_film INTEGER PRIMARY KEY AUTO_INCREMENT,
    titolo VARCHAR(60),
    regista INTEGER,
    FOREIGN KEY(regista) REFERENCES Registi(id_regista)
);


CREATE TABLE `Proiezioni`
(
	codice INTEGER PRIMARY KEY AUTO_INCREMENT,
    giorno DATE,
    ora TIME,
    film INTEGER,
    FOREIGN KEY(film) REFERENCES Film(id_film)
);


CREATE TABLE `Prenotazioni_Proiezioni`
(
	allievo VARCHAR(16),
    codice_proiezione INTEGER,
    PRIMARY KEY (allievo, codice_proiezione),
    FOREIGN KEY (allievo) REFERENCES Allievi(cf),
    FOREIGN KEY (codice_proiezione) REFERENCES Proiezioni(codice)
);


CREATE TABLE `Conferenzieri`(
	id_conferenziere INTEGER PRIMARY KEY AUTO_INCREMENT,
    cognome_conferenziere VARCHAR(30),
    nome_conferenziere VARCHAR(30)
);

CREATE TABLE `Argomenti_Conferenze`
(
    id_argomento_conf INTEGER PRIMARY KEY AUTO_INCREMENT,
	argomento VARCHAR(60),
	conferenziere INTEGER,
    FOREIGN KEY(conferenziere) REFERENCES Conferenzieri(id_conferenziere)
);


CREATE TABLE `Conferenze`
(
	codice INTEGER PRIMARY KEY AUTO_INCREMENT,
    giorno DATE,
    ora TIME,
    argomento INTEGER,
    FOREIGN KEY(argomento) REFERENCES Argomenti_Conferenze(id_argomento_conf)
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
-- Verra usata per i report, sia dalla segreteria sia dagli insegnanti 
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

-- Verra usata dagli allievi per visualizzare le lezioni private
DROP VIEW IF EXISTS Prenotazioni_Lezioni_Private;
CREATE VIEW Prenotazioni_Lezioni_Private
AS
SELECT a.cf AS CF_Allievo, a.cognome AS Cognome_Allievo, a.nome AS Nome_Allievo, 
	i.cognome AS Cognome_Insegnante, i.nome AS Nome_Insegnante, lp.giorno 
	AS Giorno, lp.ora AS Ora
FROM Allievi a JOIN Lezioni_Private  lp ON lp.allievo = a.cf
	JOIN Insegnanti i ON lp.insegnante = i.cf;

-- Le seguenti viste ricongiungono Proiezioni/Film/Registi e Conferenze/Argomenti_Conferenze/Conferenzieri
DROP VIEW IF EXISTS ProiezioniCompleta;
CREATE VIEW ProiezioniCompleta
AS
(
	SELECT  p.codice AS codice, p.giorno AS giorno, p.ora AS ora, f.titolo AS film, reg.cognome_regista AS cognome_regista, reg.nome_regista AS nome_regista
	FROM Proiezioni p JOIN Film f ON p.film = f.id_film JOIN Registi reg ON f.regista = reg.id_regista
);

DROP VIEW IF EXISTS ConferenzeCompleta;
CREATE VIEW ConferenzeCompleta
AS
(
	SELECT  c.codice AS codice, c.giorno AS giorno, c.ora AS ora, ac.argomento AS argomento, conf.cognome_conferenziere AS cognome_conferenziere, conf.nome_conferenziere AS nome_conferenziere
	FROM Conferenze c JOIN Argomenti_Conferenze ac ON c.argomento = ac.id_argomento_conf JOIN Conferenzieri conf ON ac.conferenziere = conf.id_conferenziere
);

-- Verra usata dagli allievi per consultare le proprie prenotazioni //TODO 
DROP VIEW IF EXISTS PrenotazioniAttive;
CREATE VIEW PrenotazioniAttive
AS
(SELECT pren.allievo as CF_allievo, 'Proiezione' AS Tipologia, pc.giorno AS Giorno, pc.ora AS Ora, pc.film AS TitoloArgomento, pc.cognome_regista AS CognomeAutore, pc.nome_regista AS NomeAutore
FROM Prenotazioni_Proiezioni pren JOIN ProiezioniCompleta pc ON pren.codice_proiezione = pc.codice)
UNION
(SELECT pren.allievo as CF_allievo, 'Conferenza' AS Tipologia, c.giorno AS Giorno, c.ora AS Ora, c.argomento AS TitoloArgomento, c.cognome_conferenziere AS CognomeAutore, c.nome_conferenziere AS NomeAutore
FROM Prenotazioni_Conferenze pren JOIN ConferenzeCompleta c ON pren.codice_conferenza = c.codice);


-- Procedure
DELIMITER $$
CREATE PROCEDURE Nuova_Attivita(IN TIPOLOGIA INTEGER, IN giorno_in DATE, IN ora_in TIME, IN argfilm_in VARCHAR(60), IN cognome_in VARCHAR(30), IN nome_in VARCHAR(30))
BEGIN

	DECLARE idautore INTEGER;
    DECLARE idopera INTEGER;

	-- Se è una proiezione
	IF TIPOLOGIA = 0 THEN
		-- Verifico esistenza regista sul db
        SELECT id_regista INTO idautore
        FROM Registi
        WHERE cognome_regista = cognome_in
        AND nome_regista = nome_in;
        
        IF idautore IS NULL THEN
        
			INSERT INTO Registi(cognome_regista, nome_regista)
            VALUES (cognome_in, nome_in);
            
			SET idautore = LAST_INSERT_ID();    
            
        END IF;
        
		SELECT id_film INTO idopera
		FROM Film
		WHERE titolo = argfilm_in
		AND regista = idautore;
      
		-- Verifico esistenza film sul db
        IF idopera IS NULL THEN
        
			INSERT INTO Film(titolo, regista)
            VALUES (argfilm_in, idautore);
            
            SET idopera = LAST_INSERT_ID();
        
        END IF;
        
        INSERT INTO Proiezioni(giorno, ora, film)
        VALUES (giorno_in, ora_in, idopera);
        
	-- Se è una conferenza TODO
	ELSEIF TIPOLOGIA = 1 THEN

        SELECT id_conferenziere INTO idautore
        FROM Conferenzieri
        WHERE cognome_conferenziere = cognome_in
        AND nome_conferenziere = nome_in;
        
        IF idautore IS NULL THEN
        
			INSERT INTO Conferenzieri(cognome_conferenziere, nome_conferenziere)
            VALUES (cognome_in, nome_in);
            
			SET idautore = LAST_INSERT_ID();    
            
        END IF;
        
		SELECT id_argomento_conf INTO idopera
		FROM Argomenti_Conferenze
		WHERE argomento = argfilm_in
		AND conferenziere = idautore;
      
		-- Verifico esistenza film sul db
        IF idopera IS NULL THEN
        
			INSERT INTO Argomenti_Conferenze(argomento, conferenziere)
            VALUES (argfilm_in, idautore);
            
            SET idopera = LAST_INSERT_ID();
        
        END IF;
        
        INSERT INTO Conferenze(giorno, ora, argomento)
        VALUES (giorno_in, ora_in, idopera);
	ELSE
		ROLLBACK;
	END IF;

	COMMIT;
END$$
DELIMITER ;

DELIMITER $$
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
END$$
DELIMITER ;

DELIMITER $$
CREATE PROCEDURE Aggiungi_Assenza(IN allievo_in VARCHAR(16), IN giorno_in DATE, IN ora_in TIME, IN aula_in VARCHAR(4), IN insegnante_in VARCHAR(16))
BEGIN

    -- Solo un insegnante abilitato al corso puo aggiungere una assenza
    IF NOT EXISTS  
    (SELECT d.insegnante FROM Docenze d JOIN Corsi c ON d.corso = c.codice JOIN Lezioni l ON l.corso = c.codice
    WHERE d.insegnante = insegnante_in
    AND l.giorno = giorno_in
    AND l.ora = ora_in
    AND l.aula = aula_in)

    THEN
        SIGNAL SQLSTATE '12345'
        SET MESSAGE_TEXT = 'Insegnante non abilitato al corso';
        ROLLBACK;
    ELSE
        INSERT INTO Assenze(allievo, giorno, ora, aula) VALUES (allievo_in, giorno_in, ora_in, aula_in);
	    COMMIT;
	END IF;
END$$
DELIMITER ;

DELIMITER $$
CREATE PROCEDURE Nuovo_Anno()
BEGIN

    -- Ipotizzo che Livelli ed Insegnanti rimangano da un anno all'altro
	SET FOREIGN_KEY_CHECKS = 0;
    TRUNCATE TABLE Prenotazioni_Conferenze;
    TRUNCATE TABLE Prenotazioni_Proiezioni;
    TRUNCATE TABLE Conferenze;
    TRUNCATE TABLE Proiezioni;
    TRUNCATE TABLE Argomenti_Conferenze;
    TRUNCATE TABLE Film;
    TRUNCATE TABLE Conferenzieri;
    TRUNCATE TABLE Registi;
    TRUNCATE TABLE Lezioni_Private;
    TRUNCATE TABLE Assenze;
    TRUNCATE TABLE Lezioni;
    TRUNCATE TABLE Allievi;
    TRUNCATE TABLE Docenze;
    TRUNCATE TABLE Corsi;
    SET FOREIGN_KEY_CHECKS = 1;

	COMMIT;
END$$
DELIMITER ;

-- Verra usata per i report, sia dalla segreteria sia dagli insegnanti 
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

DELIMITER $$
CREATE TRIGGER controllo_orario_corsi_trg
BEFORE INSERT
   ON Docenze FOR EACH ROW
BEGIN
    IF EXISTS
    (
        SELECT l.giorno, l.ora 
        FROM Lezioni l JOIN Corsi c 
        ON c.codice = l.corso 
        JOIN Docenze d ON d.corso = c.codice
        WHERE d.insegnante = NEW.insegnante
        AND c.codice <> NEW.corso
        INTERSECT
        SELECT l.giorno, l.ora 
        FROM Lezioni l 
        WHERE l.corso = NEW.corso 
    ) 
    THEN  
        SIGNAL SQLSTATE '12345'
        SET MESSAGE_TEXT = 'Una o piu fasce orarie non disponibili!';
    END IF;
END$$
DELIMITER ;


-- Trigger

-- Se una lezione privata cade in un orario in cui l'insegnante ha l'orario occupato, ne impedisce la prenotazione
DELIMITER $$
CREATE TRIGGER controllo_orario_lezione_privata_trg
BEFORE INSERT
   ON Lezioni_Private FOR EACH ROW
BEGIN 
    IF EXISTS
    (
        (SELECT i.cf AS CF_Insegnante, i.cognome AS Cognome, i.nome AS Nome, lp.giorno 
	    AS Giorno, lp.ora AS Ora
        FROM Insegnanti i JOIN Lezioni_Private lp ON i.cf = lp.insegnante
        WHERE i.cf = NEW.insegnante
        AND HOUR(NEW.ora) = HOUR(lp.ora)
        AND lp.giorno = NEW.giorno)
        UNION
        (SELECT i.cf AS CF_Insegnante, i.cognome AS Cognome, i.nome AS Nome, l.giorno 
	    AS Giorno, l.ora AS Ora
        FROM Insegnanti i JOIN Docenze d ON i.cf = d.insegnante 
		JOIN Corsi c ON d.corso = c.codice 
		JOIN Lezioni l ON l.corso = c.codice
        WHERE i.cf = NEW.insegnante
        AND HOUR(NEW.ora) = HOUR(l.ora)
        AND l.giorno = NEW.giorno)
    ) 
    THEN   
        SIGNAL SQLSTATE '12345'
        SET MESSAGE_TEXT = 'Orario gia occupato!';
    END IF;
END$$
DELIMITER ;

-- Verifica che l'allievo appartenga al corso, e che la lezione si sia gia verificata
DELIMITER $$
CREATE TRIGGER controllo_validita_assenza_trg
BEFORE INSERT
   ON Assenze FOR EACH ROW
BEGIN
    IF NOT EXISTS 
    (
        SELECT * FROM Lezioni l, Allievi al
        WHERE l.aula = NEW.aula AND l.giorno = NEW.giorno AND l.ora = NEW.ora AND l.corso = al.corso AND al.cf = NEW.allievo
    ) 
    THEN  
        SIGNAL SQLSTATE '12345'
        SET MESSAGE_TEXT = 'Data o allievo non validi';
    ELSEIF (NEW.giorno > NOW())
        THEN 
        SIGNAL SQLSTATE '12345'
        SET MESSAGE_TEXT = 'Lezione ancora da verificarsi';
    END IF;
END$$
DELIMITER ;

CREATE INDEX proiezioni_idx ON Proiezioni(giorno, ora);
CREATE INDEX conferenze_idx ON Conferenze(giorno, ora);

COMMIT;

GRANT SELECT, INSERT, DELETE  ON gestione_lingue_straniere.Assenze  TO 'user-insegnanti'@'localhost';
GRANT SELECT  ON gestione_lingue_straniere.Lezioni_Private  TO 'user-insegnanti'@'localhost';
GRANT SELECT  ON gestione_lingue_straniere.Lezioni  TO 'user-insegnanti'@'localhost';
GRANT SELECT  ON gestione_lingue_straniere.Insegnanti  TO 'user-insegnanti'@'localhost';
GRANT SELECT  ON gestione_lingue_straniere.Impegni_Insegnante TO 'user-insegnanti'@'localhost';
GRANT SELECT  ON gestione_lingue_straniere.Docenze TO 'user-insegnanti'@'localhost';
GRANT SELECT  ON gestione_lingue_straniere.Corsi TO 'user-insegnanti'@'localhost';
GRANT EXECUTE ON PROCEDURE gestione_lingue_straniere.Aggiungi_Assenza TO 'user-insegnanti'@'localhost';

GRANT SELECT  ON gestione_lingue_straniere.Allievi TO 'user-allievi'@'localhost';
GRANT SELECT  ON gestione_lingue_straniere.Insegnanti TO 'user-allievi'@'localhost';
GRANT SELECT  ON gestione_lingue_straniere.ProiezioniCompleta TO 'user-allievi'@'localhost';
GRANT SELECT  ON gestione_lingue_straniere.ConferenzeCompleta TO 'user-allievi'@'localhost';
GRANT SELECT  ON gestione_lingue_straniere.PrenotazioniAttive TO 'user-allievi'@'localhost';
GRANT SELECT  ON gestione_lingue_straniere.Assenze TO 'user-allievi'@'localhost';
GRANT SELECT  ON gestione_lingue_straniere.Prenotazioni_Lezioni_Private TO 'user-allievi'@'localhost';
GRANT SELECT, INSERT, DELETE  ON gestione_lingue_straniere.Prenotazioni_Conferenze TO 'user-allievi'@'localhost';
GRANT SELECT, INSERT, DELETE  ON gestione_lingue_straniere.Prenotazioni_Proiezioni TO 'user-allievi'@'localhost';
GRANT SELECT, INSERT, DELETE  ON gestione_lingue_straniere.Lezioni_Private TO 'user-allievi'@'localhost';
GRANT EXECUTE ON PROCEDURE gestione_lingue_straniere.Prenotazione_Attivita TO 'user-allievi'@'localhost';

GRANT ALL PRIVILEGES ON * . * TO 'user-segreteria'@'localhost';

FLUSH PRIVILEGES;

-- Popolazione del DB con dei dati di prova
INSERT INTO Livelli(denominazione, titolo_libro, esame_necessario) VALUES('Elementary', 'English for beginners', FALSE);

INSERT INTO Livelli(denominazione, titolo_libro, esame_necessario) VALUES('Intermediate', 'Cambridge Book', FALSE);

INSERT INTO Livelli(denominazione, titolo_libro, esame_necessario) VALUES('First Certificate', 'Oxford Workbook', TRUE);

INSERT INTO Livelli(denominazione, titolo_libro, esame_necessario) VALUES('Advanced', 'Oxford Workbook 2', TRUE);

INSERT INTO Livelli(denominazione, titolo_libro, esame_necessario) VALUES('Proficiency', 'Proficiency Workbook', TRUE);

INSERT INTO Insegnanti(cf, pwd, cognome, nome, indirizzo, nazione) VALUES('INSEGNANTE000000', MD5('password'), 'Brock', 'Boris', 'Oxford Street 24', 'UK');
INSERT INTO Insegnanti(cf, pwd, cognome, nome, indirizzo, nazione) VALUES('INSEGNANTE000001', md5('password'), 'Taylor', 'Vincent', 'Cook Square 99', 'AU');
INSERT INTO Insegnanti(cf, pwd, cognome, nome, indirizzo, nazione) VALUES('INSEGNANTE000002', md5('password'), 'Smith', 'Robert', 'Grafton Street 101', 'IE');

INSERT INTO Corsi(Livello, data_attivazione) VALUES('Elementary', NOW());
INSERT INTO Corsi(Livello, data_attivazione) VALUES('Intermediate', NOW());
INSERT INTO Corsi(Livello, data_attivazione) VALUES('Advanced', NOW());

INSERT INTO Allievi(cf, pwd, cognome, nome, sesso, data_nascita, luogo_nascita, indirizzo, corso, data_iscrizione) VALUES('ALLIEVO000000000', MD5('password'), 'Bianchi', 'Valentino', 'M', '1996-10-3', 'Roma', 'Via Tuscolana 300', 1, CURDATE());
INSERT INTO Allievi(cf, pwd, cognome, nome, sesso, data_nascita, luogo_nascita, indirizzo, corso, data_iscrizione) VALUES('ALLIEVO000000001', MD5('password'), 'Rossi', 'Maria', 'F','1997-3-25', 'Milano', 'Via Casilina 150', 2, CURDATE());
INSERT INTO Allievi(cf, pwd, cognome, nome, sesso, data_nascita, luogo_nascita, indirizzo, corso, data_iscrizione) VALUES('ALLIEVO000000002', MD5('password'), 'Verdi', 'Stefano', 'M','1995-11-2','Firenze', 'Viale Marconi 45', 2, CURDATE());

-- Settimana che va dal 9-30 al 10-4
INSERT INTO Lezioni(aula, giorno, ora, corso) VALUES ("B2", '2019-9-30', '8:00:00', 1);
INSERT INTO Lezioni(aula, giorno, ora, corso) VALUES ("A4", '2019-10-2', '8:00:00', 1);
INSERT INTO Lezioni(aula, giorno, ora, corso) VALUES ("3", '2019-10-4', '8:00:00', 1);
INSERT INTO Lezioni(aula, giorno, ora, corso) VALUES ("C4", '2019-9-30', '14:00:00', 2);
INSERT INTO Lezioni(aula, giorno, ora, corso) VALUES ("B15", '2019-10-2', '14:00:00', 2);
INSERT INTO Lezioni(aula, giorno, ora, corso) VALUES ("B15", '2019-10-4', '14:00:00', 2);

INSERT INTO Docenze(insegnante, corso) VALUES ('INSEGNANTE000000', 1);
INSERT INTO Docenze(insegnante, corso) VALUES ('INSEGNANTE000001', 1);
INSERT INTO Docenze(insegnante, corso) VALUES ('INSEGNANTE000002', 2);

INSERT INTO Lezioni(aula, giorno, ora, corso) VALUES ("H3", '2019-8-22', '8:00:00', 1);
INSERT INTO Lezioni(aula, giorno, ora, corso) VALUES ("H2", '2019-8-21', '8:00:00', 1);

INSERT INTO Assenze(allievo, giorno, ora, aula) VALUES ('ALLIEVO000000000', '2019-8-22', '8:00:00', 'H3');

CALL `gestione_lingue_straniere`.`Nuova_Attivita`(0, '2019-10-4', '14:00:00', 'Orwell 1984', 'Radford', 'Michael');
CALL `gestione_lingue_straniere`.`Nuova_Attivita`(1, '2019-10-4', '8:00:00', 'About English Language', 'Reds', 'Mario');

CALL `gestione_lingue_straniere`.`PRENOTAZIONE_ATTIVITA`(0, 'ALLIEVO000000000', 1);
INSERT INTO Lezioni_Private(insegnante, allievo, giorno, ora) VALUES('INSEGNANTE000000', 'ALLIEVO000000000', '2019-10-3', '9:00:00');

COMMIT;