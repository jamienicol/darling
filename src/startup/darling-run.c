/*
This file is part of Darling.

Copyright (C) 2026 Darling developers

Darling is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Darling is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Darling.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * darling-run: run a single Mach-O program without a container.
 *
 * Unlike `darling`, this is NOT setuid root and creates no namespaces, no mounts and no launchd.
 * It just hands off to darlingserver's standalone mode, which serves this one program's Mach
 * traps in-process and exits when it does. See src/external/darlingserver/src/standalone.cpp.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char* server_path = INSTALL_PREFIX "/bin/darlingserver";

int main(int argc, char** argv)
{
	char** args;
	int i;

	if (argc < 2)
	{
		fprintf(stderr,
			"usage: %s [--root <path>] <program> [args...]\n"
			"\n"
			"Run a Mach-O program with no container: no root privileges, no namespaces, no\n"
			"mounts, and no launchd. Suitable for self-contained command line programs.\n"
			"Programs that need XPC or a bootstrap lookup require `darling shell` instead.\n"
			"\n"
			"<program> may be a path on the Linux filesystem, a path inside the Darling root,\n"
			"or a path relative to it (e.g. /bin/echo).\n",
			argv[0]);
		return 1;
	}

	/* server_path, "--standalone", argv[1..argc-1], NULL */
	args = (char**) malloc(sizeof(char*) * (argc + 2));
	if (!args)
	{
		fprintf(stderr, "%s: out of memory\n", argv[0]);
		return 1;
	}

	args[0] = (char*) server_path;
	args[1] = "--standalone";
	for (i = 1; i < argc; i++)
		args[i + 1] = argv[i];
	args[argc + 1] = NULL;

	execv(server_path, args);

	fprintf(stderr, "%s: failed to execute %s: %s\n", argv[0], server_path, strerror(errno));
	return 1;
}
