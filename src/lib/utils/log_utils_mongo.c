/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2024, Carlos Gonzalez Cortes, carlgonz@uchile.cl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "suchai/log_utils_mongo.h"

// Declared in repoData.h
extern time_t dat_get_time(void);

int mongodb_log_init(void)
{
    char uri[SCH_BUFF_MAX_LEN] = {0};
    mongoc_client_t *client = NULL;
    bson_error_t error = {0};
    mongoc_server_api_t *api = NULL;
    mongoc_database_t *database = NULL;
    bson_t *command = NULL;
    bson_t reply = BSON_INITIALIZER;
    int rc = 0;
    bool ok = true;

    // Initialize the MongoDB C Driver.
    mongoc_init();

    // Build URI
    char *user = getenv("MONGO_USER_NAME");
    char *pass = getenv("MONGO_PASSWORD");
    if(user != NULL && pass != NULL)
    {
        int n = snprintf(uri, SCH_BUFF_MAX_LEN-1, "mongodb://%s:%s@%s:%d/?authSource=%s", user, pass, SCH_LOG_MONGODB_HOST, SCH_LOG_MONGODB_PORT, SCH_LOG_MONGODB_NAME);
        assert (n < SCH_BUFF_MAX_LEN);
    }
    else
    {
        int n = snprintf(uri, SCH_BUFF_MAX_LEN-1, "mongodb://%s:%d", SCH_LOG_MONGODB_HOST, SCH_LOG_MONGODB_PORT);
        assert (n < SCH_BUFF_MAX_LEN);
    }

    printf("Connecting to %s\n", uri);
    client = mongoc_client_new(uri);
    if (!client) {
        fprintf(stderr, "Failed to create a MongoDB client.\n");
        rc = -1;
        goto cleanup;
    }

    // Set the version of the Stable API on the client.
    api = mongoc_server_api_new(MONGOC_SERVER_API_V1);
    if (!api) {
        fprintf(stderr, "Failed to create a MongoDB server API.\n");
        rc = -1;
        goto cleanup;
    }

    ok = mongoc_client_set_server_api(client, api, &error);
    if (!ok) {
        fprintf(stderr, "error: %s\n", error.message);
        rc = -1;
        goto cleanup;
    }

    // Get a handle on the "admin" database.
    database = mongoc_client_get_database(client, SCH_LOG_MONGODB_NAME);
    if (!database) {
        fprintf(stderr, "Failed to get a MongoDB database handle.\n");
        rc = -1;
        goto cleanup;
    }

    // Ping the database.
    command = BCON_NEW("ping", BCON_INT32(1));
    ok = mongoc_database_command_simple(database, command, NULL, &reply, &error);
    if (!ok) {
        fprintf(stderr, "error: %s\n", error.message);
        rc = -1;
        goto cleanup;
    }

    printf("Pinged your deployment. You successfully connected to MongoDB!\n");

    bson_destroy(&reply);
    bson_destroy(command);
    mongoc_server_api_destroy(api);
    mongo_log_driver.client = client;
    return rc;

// Perform cleanup.
cleanup:
    bson_destroy(command);
    mongoc_database_destroy(database);
    mongoc_server_api_destroy(api);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return rc;
}

int mongodb_log(const char *lvl, const char *tag, const char *msg, ...)
{
    assert(mongo_log_driver.client != NULL);

    bson_t *document;
    document = bson_new();

    // Add tag, lvl and datetime
    BSON_APPEND_UTF8(document, "id", tag);
    BSON_APPEND_UTF8(document, "log_type", lvl);
    BSON_APPEND_INT64(document, "time", (unsigned long)dat_get_time());

    // Add message
    bson_string_t *str = bson_string_new(NULL);
    va_list args;
    va_start(args, msg);
    bson_string_append_printf(str, msg, args);
    va_end(args);
    BSON_APPEND_UTF8(document, "data", str->str);
    bson_string_free(str, true);

    // Save to mongo
    bson_error_t error;
    mongoc_collection_t *collection = mongoc_client_get_collection (mongo_log_driver.client, SCH_LOG_MONGODB_NAME, "collection");
    if (!mongoc_collection_insert_one (collection, document, NULL, NULL, &error))
    {
        fprintf (stderr, "%s\n", error.message);
    }

    /*
    * Print the document as a JSON string.
    */
   // char *str = bson_as_canonical_extended_json (document, NULL);
   // printf ("===\n%s\n===\n", str);
   // bson_free (str);

   /*
    * Clean up allocated bson documents.
    */
    bson_destroy (document);
    mongoc_collection_destroy(collection);
    return 0;
}