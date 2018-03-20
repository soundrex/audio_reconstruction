#include <fcntl.h>
#include <unistd.h>
#include <unix/soundrex/common.h>
#include <iostream>
#include <string>

int main(int argc, char **argv) try {
	if (argc < 2)
		throw std::runtime_error(std::string("Usage: ") + argv[0] + "<music_file>");

	send_stdin();
	int pipes[2];
	wrap_error(pipe2(pipes, O_CLOEXEC), "creating pipes");
	wrap_error(dup2(pipes[0], 0), "duplicating stdin");

	if (!wrap_error(fork(), "forking"))
		wrap_error(execlp("./throttle", argv[0], nullptr), "opening throttle");

	std::array<char const *, 2> const ffend{{"-", nullptr}};
	char const *vals[ffmpeg_init.size() + ffmpeg_params.size() + 1 + argc];
	copy_all<char const *>(vals, {ffmpeg_init, {argv + 1, argv + argc}, ffmpeg_params, ffend});

	wrap_error(dup2(open("/dev/null", O_CLOEXEC), 0), "opening /dev/null");
	wrap_error(dup2(pipes[1], 1), "duplicating stdout");
	wrap_error(execvp("ffmpeg", (char **)vals), "Opening ffmpeg");
} catch (std::runtime_error const &err) {
	std::cerr << argv[0] << ": " << err.what() << '\n';
	return 1;
}