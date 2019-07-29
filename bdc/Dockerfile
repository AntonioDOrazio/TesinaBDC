FROM mariadb
MAINTAINER Alessandro Pellegrini <pellegrini@diag.uniroma1.it>

RUN mkdir -p /bdc
WORKDIR /bdc
COPY . .

RUN apt-get update
RUN apt-get install -y gcc make automake autoconf libncurses-dev libmariadbd-dev libssl-dev procps #libmariadbclient-dev

CMD ./run.sh
