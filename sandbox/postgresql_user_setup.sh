#!/usr/bin/env bash

#
# Script for setting up the postgres user SUCHAI_Flight_Software
# manages it's database from when in storage mode 2
#
# Needs to be run by the 'postgres' Linux user
#
# Meant to be executed by the 'database_setup.sh' script
#
# Authors: Diego Ortego P
#          Camilo Rojas

# Checks if arguments were passed
if [ $# -ne 1 ]; then
    echo $0: usage: myscript name
    # If no arguments were passed
    exit 1
fi

# Takes the first argument as the username to give postgres superuser privileges to
postgresql_user=$1

# Creates the user
if psql -t -c '\du' | cut -d \| -f 1 | grep -qw "$postgresql_user"; then
    echo "- Postgresql user '$postgresql_user' already exists"
else
    echo "- Creating postgresql user '$postgresql_user'..."
    createuser "$postgresql_user"
fi

# Gives the user superuser privileges
if psql -t -c '\du' | grep "$postgresql_user" | cut -d \| -f 2 | grep -qw "Superuser"; then
    echo "- Postgresql user '$postgresql_user' already is a superuser"
else
    echo "- Giving postgresql user '$postgresql_user' superuser privileges..."
    psql -q -c "ALTER USER $postgresql_user WITH SUPERUSER;"
fi

# Creates a default personal database for the user to enter postgres into
if psql -t -c '\l' | cut -d \| -f 1 | grep -qw "$postgresql_user"; then
    echo "- Postgresql database '$postgresql_user' already exists"
else
    echo "- Creating postgresql database '$postgresql_user'..."
    createdb "$postgresql_user"
fi

exit 0
