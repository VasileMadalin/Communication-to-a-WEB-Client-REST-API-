#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params, 
    char **cookies, int cookies_count, char * authorization) {
    char *message = malloc(BUFLEN*sizeof(char));
    char *line = malloc(LINELEN*sizeof(char));

    // se adauga numele metodei, url-ul si alti parametrii daca este necesar
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    // se adauga linia creata in cerere
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);

    // se adauga in cerere
    compute_message(message, line);

    // daca exista cookie-uri, se adauga in cerere
    if (cookies != NULL) {
        // se parcurg toate cookie-urile si se adauga
        for (int p = 0; p < cookies_count; p++) {
            memset(line, 0, LINELEN);
            sprintf(line, "Cookie: %s", cookies[p]);

            compute_message(message, line);
        }
    }
    // se adauga si header-ul de autorizatie, daca este nevoie
    if (authorization != NULL) {
        compute_message(message, authorization);
    }
    // se adauga caracterele speciale la final ce cerere
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, 
    char **body_data, int body_data_fields_count, char **cookies, 
    int cookies_count, char *authorization) {
    char *message = malloc(BUFLEN*sizeof(char));
    char *line = malloc(LINELEN*sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // se scrie numele metodei, URL, si protocolul
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // se adauga host-ul
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);

    // se adauga o linie noua in cerere
    compute_message(message, line);

    // se adauga headere
    strcpy(body_data_buffer, "");
    for (int p = 0; p < body_data_fields_count; p++) {
        strcat(body_data_buffer, body_data[p]);
        if (p != body_data_fields_count - 1) {
            strcat(body_data_buffer, "&");
        }
    }

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);

    // se adauga cookie-urile
    if (cookies != NULL) {
       for (int p = 0; p < cookies_count; p++) {
            memset(line, 0, LINELEN);
            sprintf(line, "Cookie: %s", cookies[p]);

            compute_message(message, line);
        }
    }
    // se adauga header-ul de autorizatie de este necesar
    if (authorization != NULL) {
        compute_message(message, authorization);
    }
    // se adauga linie noua la finalul cererii
    compute_message(message, "");
    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    return message;
}
char *compute_delete_request(char *host, char *url, char * authorization)
{
    char *message = malloc(BUFLEN*sizeof(char));
    char *line = malloc(LINELEN*sizeof(char));

    // se adauga numele metode, URL, si alti parametrii
    sprintf(line, "DELETE %s HTTP/1.1", url);
    // se adauga linia la cerere
    compute_message(message, line);

    memset(line, 0, LINELEN);
    // se adauga host-ul la linie
    sprintf(line, "Host: %s", host);

    compute_message(message, line);

    // se adauga header-ul de autorizatie daca este nevoie
    if (authorization != NULL) {
        compute_message(message, authorization);
    }
    // se adauga linie noua la sfarsitul cererii
    compute_message(message, "");
    return message;
}