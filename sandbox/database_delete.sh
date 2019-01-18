#!/usr/bin/env bash

#
# Script for deleting the database SUCHAI_Flight_Software uses when in
# storage mode 2
#
# Doesn't edit already existent postgres users nor privileges
#
# Authors: Diego Ortego P
#

# Deletes the 'fs_db' database from SUCHAI_Flight_Software's third storage mode
if psql -t -c '\l' | cut -d \| -f 1 | grep -qw "fs_db"; then
    echo "Deleting postgresql database 'fs_db'..."
    dropdb fs_db
else
    echo "Postgresql database 'fs_db' doesn't exist"
fi
