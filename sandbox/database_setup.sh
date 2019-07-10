#!/usr/bin/env bash

#
# Script for setting up the database SUCHAI_Flight_Software uses when in
# storage mode 2
#
# Authors: Diego Ortego P
#          Camilo Rojas

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

exit 0
