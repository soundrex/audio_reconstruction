#include <unistd.h>

#include <soundrex/unix/ffmpeg.hpp>
#include <soundrex/unix/runtime/lib.hpp>

void soundrex_main(std::span<char *const>) {
	std::array<char const *, 2> const in({"-", nullptr});
	char const *arr[ffplay_init.size() + ffmpeg_params.size() + in.size()];
	copy_all<char const *const>(arr, {ffplay_init, ffmpeg_params, in});
	execvp("ffplay", const_cast<char **>(arr));
	wrap_error(-1, "Couldn't play");
}
