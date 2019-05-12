/*
 * Copyright © 2019 Manuel Stoeckl
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <wayland-client-core.h>

int run_client(const char *socket_path)
{
	struct wl_display *display = wl_display_connect(NULL);
	if (!display) {
		fprintf(stderr, "Failed to connect to a wayland server.\n");
		return EXIT_FAILURE;
	}

	struct sockaddr_un saddr;
	int fd;

	if (strlen(socket_path) >= sizeof(saddr.sun_path)) {
		fprintf(stderr, "Socket path is too long and would be truncated: %s\n",
				socket_path);
		return EXIT_FAILURE;
	}

	saddr.sun_family = AF_UNIX;
	strncpy(saddr.sun_path, socket_path, sizeof(saddr.sun_path) - 1);
	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1) {
		fprintf(stderr, "Error creating socket: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	if (bind(fd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
		fprintf(stderr, "Error binding socket: %s\n", strerror(errno));
		close(fd);
		return EXIT_FAILURE;
	}

	if (listen(fd, 1) == -1) {
		fprintf(stderr, "Error listening to socket: %s\n",
				strerror(errno));
		close(fd);
		unlink(socket_path);
		return EXIT_FAILURE;
	}

	fprintf(stderr, "I'm a client on %s!\n", socket_path);
	for (int i = 0; i < 1; i++) {
		// Q: multiple parallel client support?

		int client = accept(fd, NULL, NULL);
		if (client == -1) {
			fprintf(stderr, "Skipping connection\n");
			continue;
		}

		int bufsize = 4096;
		char *buf = calloc(bufsize + 1, 1);
		while (1) {
			int nb = read(client, buf, bufsize);
			if (nb <= 0) {
				fprintf(stderr, "Read failed, stopping\n");
				break;
			} else {
				fprintf(stderr, "Read with %d bytes of data |%s|\n",
						nb, buf);
			}
		}
		fprintf(stderr, "...\n");
	}

	close(fd);
	unlink(socket_path);

	return EXIT_SUCCESS;
}
