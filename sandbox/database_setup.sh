#!/usr/bin/env bash

#
# Script for setting up the database SUCHAI_Flight_Software uses when in
# storage mode 2
#
# Authors: Diego Ortego P
#

# Gets the current execution directory (the absolute path to this script)
SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"
export WORKSPACE=${SCRIPT_PATH}/..

# Prints the current workspace path, for debugging purposes
echo "The current workspace is: ${WORKSPACE}"

# Adds the current user to the 'postgres' group
echo "Setting up user $USER..."
if groups $USER | grep -qw "postgres"; then
    echo "- User $USER is already a member of the 'postgres' group"
else
    echo "- Adding user $USER to the 'postgres' group..."
    sudo usermod -a -G postgres $USER
fi
echo "Done"

# Sets up the current user as a superuser in postgresql from the postgres default user
echo "Setting up postgresql user $USER..."
sudo su - postgres -c "bash ${WORKSPACE}/sandbox/postgresql_user_setup.sh ${USER}"
echo "Done"

echo "Setting up postgresql database 'fs_db'..."

# Creates the 'fs_db' database for SUCHAI_Flight_Software's third storage mode
if psql -t -c '\l' | cut -d \| -f 1 | grep -qw "fs_db"; then
    echo "- Postgresql database 'fs_db' already exists"
else
    echo "- Creating postgresql database 'fs_db'..."
    createdb "fs_db"
fi

# Gives the current user al privileges on the 'fs_db' database
if psql -t -c '\l' | sed -n '/fs_db/,/^     /p' | cut -d \| -f 6 | grep "$USER" | grep "c" | grep "C" | grep -qw "T"; then
    echo "- Postgresql user $USER already has all privileges on database 'fs_db'"
else
    echo "- Giving postgresql user '$USER' all privileges on database 'fs_db'..."
    psql -q -c "GRANT ALL PRIVILEGES ON DATABASE fs_db TO $USER WITH GRANT OPTION;"
fi

echo "Done"
