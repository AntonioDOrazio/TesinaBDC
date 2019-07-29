#/bin/bash


echo "Initializing DBMS..."
MYSQL_ROOT_PASSWORD=bdc /docker-entrypoint.sh mysqld &> /dev/null  &

while ! mysqladmin ping -pbdc --silent; do
    sleep 1
done
sleep 5

cd $STUDENTE

echo "Compiling application..."
make

echo "Running db.sql script..."
mysql -pbdc < db.sql

echo "Testing application..."
./applicazione
