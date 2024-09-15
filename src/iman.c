#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "iman.h"

#define BUFFER_SIZE 2048

// Helper function to strip HTML tags from the response
void strip_html_tags(char *response) {
    int in_tag = 0;
    while (*response) {
        if (*response == '<') {
            in_tag = 1;
        } else if (*response == '>') {
            in_tag = 0;
        } else if (!in_tag) {
            putchar(*response);
        }
        response++;
    }
}

// Helper function to skip HTTP headers and return only the body content
char* skip_headers(char *response) {
    char *content_start = strstr(response, "\r\n\r\n");
    if (content_start) {
        return content_start + 4;  // Skip the header's end marker
    }
    return response;
}

void fetch_man_page(const char* command) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    char buffer[BUFFER_SIZE];
    int numbytes;
    char request[BUFFER_SIZE];

    // Prepare the hints for the getaddrinfo function
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // Resolve the hostname to an IP address
    if (getaddrinfo("man.he.net", "80", &hints, &servinfo) != 0) {
        perror("getaddrinfo");
        return;
    }

    // Loop through all the results and connect to the first one we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        printf("Failed to connect to man.he.net\n");
        return;
    }

    freeaddrinfo(servinfo); // Free the linked list

    // Create the HTTP GET request to fetch the man page for the command
    snprintf(request, sizeof(request), "GET /man1/%s HTTP/1.1\r\nHost: man.he.net\r\nConnection: close\r\n\r\n", command);

    // Send the GET request
    if (send(sockfd, request, strlen(request), 0) == -1) {
        perror("send");
        close(sockfd);
        return;
    }

    int headers_skipped = 0;  // Flag to mark that headers have been fully skipped
    char* processed_data;

    // Receive the response and process it
    while ((numbytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[numbytes] = '\0';  // Null-terminate the buffer

        if (!headers_skipped) {
            // Skip headers
            processed_data = skip_headers(buffer);
            headers_skipped = 1;
            strip_html_tags(processed_data);  // Strip HTML tags from the remaining content
        } else {
            strip_html_tags(buffer);  // Process the rest of the content
        }
    }

    if (numbytes == -1) {
        perror("recv");
    }

    close(sockfd);
}
