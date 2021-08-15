// Vasile Madalin Constantin 322CB
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"
// functie care extrage cookie-ul din raspunsul unei cereri
char * get_cookie(char * response) {
	int n = strlen(response);
	char * cookie = (char *)malloc(LINELEN * sizeof(char ));
	for (int i = 0; i < n; i++) {
		if (response[i] == '\n') {
			int k = i + 1;
			if (response[k] == 'S' && response[k + 1] == 'e' && 
				response[k + 2] == 't') {
				int j;
				for(j = k + 12; response[j] != ';'; j++) {
					cookie[j - k - 12] = response[j];
				}
				cookie[j - k - 12] = '\0';
				return cookie;
				break;
			}
			
		}
	}
	return NULL;
}
// functie care extrage token-ul din raspunsul cererii
char * get_token(char * body_response) {
	int n = strlen(body_response);
	char * token = malloc(LINELEN * sizeof(char));
	strcpy(token, "null");
	for(int i = 10; i < n - 2; i++) {
		token[i - 10] = body_response[i];
	}
	token[n - 2 - 10] = '\0';
	return token;
}
// functie care afiseaza lista de carti
void write_books(char * response) {
	char *mesaj = basic_extract_json_response(response);
	if (mesaj != NULL) {
		int n = strlen(mesaj);
		int k = 0;
		char *jason = (char*)malloc(30*sizeof(char));
		for(int i = 0; i < n; i++) {
			if (mesaj[i] == '{') {
				k = 0;
			}
			jason[k] = mesaj[i];
			k ++;
			if (mesaj[i] == '}') {
				jason[k] = '\0';
				char * serialized_string = NULL;
				JSON_Value *root_value = json_parse_string(jason);
				serialized_string = json_serialize_to_string_pretty(root_value);
				puts(serialized_string);
			}
		}
	}
	else {
		printf("There are no books\n");
	}
}
// functie care afiseaza un obiect json
void show_json(char * mesaj) {
	char * serialized_string = NULL;
	JSON_Value *root_value = json_parse_string(mesaj);
	serialized_string = json_serialize_to_string_pretty(root_value);
	puts(serialized_string);
}
// functie care citeste numele si parola pentru register si login
// si creeaza un obiect json
void read_name_password(char **data) {
	data[0] = malloc(BUFLEN);

	printf("username==");
	char username[RANDOM_STRING_VALUE];
	int n;
	fgets(username, RANDOM_STRING_VALUE, stdin);
	fgets(username, RANDOM_STRING_VALUE, stdin);
	n = strlen(username);
	username[n - 1] = '\0';
	printf("password==");
	char password[RANDOM_STRING_VALUE];
	fgets(password, RANDOM_STRING_VALUE, stdin);
	n = strlen(password);
	password[n - 1] = '\0';
	sprintf(data[0], "{\"username\":\"%s\", \"password\":\"%s\"}", username, password);
}
// functie care citeste informatiile despre o carte si creeaza un obiect json
void read_book_info(char ** payload) {
	printf("title==");
	char title[RANDOM_STRING_VALUE];
	fgets(title, RANDOM_STRING_VALUE, stdin);
	fgets(title, RANDOM_STRING_VALUE, stdin);
	int n;
	n = strlen(title);
	title[n - 1] = '\0';

	char author[RANDOM_STRING_VALUE];
	printf("author==");
	fgets(author, RANDOM_STRING_VALUE, stdin);
	n = strlen(author);
	author[n - 1] = '\0';

	char genre[RANDOM_STRING_VALUE];
	printf("genre==");
	fgets(genre, RANDOM_STRING_VALUE, stdin);
	n = strlen(genre);
	genre[n - 1] = '\0';

	char publisher[RANDOM_STRING_VALUE];
	printf("publisher==");
	fgets(publisher, RANDOM_STRING_VALUE, stdin);
	n = strlen(publisher);
	publisher[n - 1] = '\0';

	printf("page_count==");
	char page_count[RANDOM_STRING_VALUE];
	fgets(page_count, RANDOM_STRING_VALUE, stdin);
	n = strlen(page_count);
	page_count[n - 1] = '\0';

	payload[0] = malloc(BUFLEN);
	sprintf(payload[0], "{\"title\":\"%s\", \"author\":\"%s\", \"genre\":\"%s\", \"publisher\":\"%s\", \"page_count\":\"%s\"}",
		title, author, genre, publisher, page_count);
}

int main(int argc, char *argv[]) {
	char *message;
	int sockfd;
	char *formEncoded = "application/json";
	char * cookie;
	char * response;
	char command[RANDOM_STRING_VALUE];
	int active = 1;
	char * token;
	// se primeste comenzi pana la introducerea lui "Exit"
	while(active == 1) {
		int check_command = 0;
		sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
		scanf("%s", command);
		// cere post pentru register
		if (strcmp(command, "register") == 0) {
			check_command = 1;
			char **data = malloc(sizeof(char*));
			// functie care citeste de la tastatura si transforma in json
			read_name_password(data);
			message = compute_post_request("34.118.48.238",
				"/api/v1/tema/auth/register",formEncoded, data, 1, NULL, 0, NULL);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			char * body_response = basic_extract_json_response(response);
			// se verifica daca sunt mesaje de eroare
			if (body_response != NULL) {
				show_json(body_response);
			} else {
				printf("Account successfully created\n");
			}
			// se elibereaza memoria
			free(data[0]);
			free(data);
		}
		// cere post pentru login
		if (strcmp(command, "login") == 0) {
			check_command = 1;
			char **data = malloc(sizeof(char*));
			// // functie care citeste de la tastatura si transforma in json
			read_name_password(data);
			message = compute_post_request("34.118.48.238", 
				"/api/v1/tema/auth/login", formEncoded, data, 1, NULL, 0, NULL);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			char * body_response = basic_extract_json_response(response);
			// se verifica daca sunt mesaje de eroare
			if (body_response != NULL) {
				show_json(body_response);
			} else {
				printf("Successfully logged in\n");
				// se extrage cookie-ul din raspunsul cererii
				cookie = get_cookie(response);
			}
		}
		// cerere get pentru accesul in librarie
		if (strcmp(command, "enter_library") == 0) {
			check_command = 1;
			if (cookie != NULL) {
				char **cookiesAuth = malloc(sizeof(char*));
				cookiesAuth[0] = malloc(strlen(cookie) + 1);
				strcpy(cookiesAuth[0], cookie);
				message = compute_get_request("34.118.48.238", 
					"/api/v1/tema/library/access", NULL, cookiesAuth, 1, NULL);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
				char * body_response = basic_extract_json_response(response);
				// se extrage token-ul din raspuns;
				token = get_token(body_response);
				if (token != NULL) {
					printf("Successfully entered to library\n");
				}
			} else {
				// se analizeaza cazul in care nu avem cookie disponibil
				message = compute_get_request("34.118.48.238", 
					"/api/v1/tema/library/access", NULL, NULL, 0, NULL);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
				char * body_response = basic_extract_json_response(response);
				if (body_response != NULL) {
					show_json(body_response);
				}      
			}
		}
		// cerere get pentru logout
		if (strcmp(command, "logout") == 0) {
			check_command = 1;
			if (cookie != NULL) {
				int cookiesAuthCount = 1;
				char **cookiesAuth = malloc(cookiesAuthCount * sizeof(char*));
				cookiesAuth[0] = malloc(strlen(cookie) + 1);
				strcpy(cookiesAuth[0], cookie);
				message = compute_get_request("34.118.48.238",
					"/api/v1/tema/auth/logout", NULL, cookiesAuth,
					cookiesAuthCount, NULL);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
				char * body_response = basic_extract_json_response(response);
				// se afiseaza eroare, daca exista
				if (body_response != NULL) {
					show_json(body_response);
				}
				else {
					printf("Successfully logged out\n");
				}
				free(cookiesAuth[0]);
				free(cookiesAuth);
			} else {
				printf("User is not connected\n");
			}
		}
		// cerere get pentru get_books
		if (strcmp(command, "get_books") == 0) {
			check_command = 1;
			char *line = calloc(LINELEN, sizeof(char));
			// se construieste headerul de autorizatie
			sprintf(line, "Authorization: bearer %s", token);
			message = compute_get_request("34.118.48.238",
				"/api/v1/tema/library/books", NULL, NULL, 0, line);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			// se afiseaza cartile sau eroare, folosind functia write_books
			write_books(response);
			free(line);
		}
		// cerere get pentru get_book
		if (strcmp(command, "get_book") == 0) {
			check_command = 1;
			printf("id==");
			int id;
			scanf("%d", &id);
			char * url = (char *)malloc(LINELEN * sizeof(char));
			sprintf(url, "/api/v1/tema/library/books/%d", id);
			char *line = malloc(LINELEN * sizeof(char));
			// se creeaza header-ul de autorizatie 
			sprintf(line, "Authorization: bearer %s", token);
			message = compute_get_request("34.118.48.238", url, NULL, NULL, 0, line);
			send_to_server(sockfd, message);
			
			response = receive_from_server(sockfd);
			char * body_response = basic_extract_json_response(response);
			// se afiseaza mesaj de eroare, daca exista
			if (body_response != NULL) {
				show_json(body_response);
			}
			free(url);
			free(line);
		}
		// cerere delete pentru delete_book
		if (strcmp(command, "delete_book") == 0) {
			check_command = 1;
			printf("id==");
			int id;
			scanf("%d", &id);
			char * url = (char *)malloc(LINELEN * sizeof(char));
			sprintf(url, "/api/v1/tema/library/books/%d", id);
			char *line = malloc(LINELEN * sizeof(char));
			// se creeaza header-ul de autorizatie
			sprintf(line, "Authorization: bearer %s", token);
			message = compute_delete_request("34.118.48.238", url, line);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			char * body_response = basic_extract_json_response(response);
			// se afiseaza mesaj de eroare, daca exista
			if (body_response != NULL) {
				show_json(body_response);
			} else {
				printf("Book succesfully delete\n");
			}
			free(url);
			free(line);
		}
		if (strcmp(command, "add_book") == 0) {
			check_command = 1;
			char **payload = malloc(sizeof(char*));
			read_book_info(payload);                
			char *line = malloc(LINELEN * sizeof(char));
			// se creeaza header-ul de autorizatie
			sprintf(line, "Authorization: bearer %s", token);
			message = compute_post_request("34.118.48.238",
				"/api/v1/tema/library/books", formEncoded, payload, 1, NULL, 0, line);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			char * body_response = basic_extract_json_response(response);
			//se afiseaza masaj de eroare, daca exista
			if (body_response != NULL) {
				show_json(body_response);
			} else {
				printf("Book succesfully added\n");
			}
			free(line);
			free(payload[0]);
			free(payload);
		}
		// comanda de exist
		if (strcmp(command, "Exit") == 0) {
			check_command = 1;
			active = 0;
		}
		if (check_command == 0) {
			printf("Wrong command\n");
		}
		close_connection(sockfd);

	}
	free(message);
	free(response);
	return 0;
}
