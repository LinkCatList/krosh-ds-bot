<img src="img/image.png" width="40%" align="right">

## Description 🍉
discord bot that implements a watermelon farm and replies to some trigger message prefixes

## Install packages 🍉
```
sudo dnf install cmake g++ 
libssl-dev zlib1g-dev 
pkg-config libpqxx-dev
```

## Start bot 🍉
```
systemctl start docker
sudo docker run --name=aboba-db -e POSTGRES_PASSWORD='qwerty' -p 1488:5432 -d --rm postgres
migrate -path ./src/schema -database 'postgres://postgres:qwerty@localhost:1488/postgres?sslmode=disable' up
./src/init-database.sh
./build.sh
./build/krash-bot
```