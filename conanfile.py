from conan import ConanFile
from conan.tools.cmake import cmake_layout


class Meridian(ConanFile):
    """Dependencies that are awkward to obtain by hand.

    Qt is deliberately NOT here: the ConanCenter Qt recipe has a large option
    matrix and routinely misses its binary cache, which turns a first build
    into a multi-hour source build. Install Qt from the official binaries
    instead (see README) -- it is one command with aqtinstall.
    """

    name = "meridian"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("ffmpeg/7.1.5")

    def configure(self):
        # We demux MP4/WAV and decode H.264/AAC, all of which are native to
        # FFmpeg. Every external library in the default option set is an
        # encoder or a device backend we never call, so they are switched
        # off: it takes the graph from ~58 packages down to a handful.
        ff = self.options["ffmpeg"]
        ff.with_programs = False
        for opt in ("with_zlib", "with_bzip2", "with_lzma", "with_libiconv",
                    "with_freetype", "with_openjpeg", "with_openh264",
                    "with_opus", "with_vorbis", "with_zeromq", "with_sdl",
                    "with_libx264", "with_libx265", "with_libvpx",
                    "with_libmp3lame", "with_libfdk_aac", "with_libwebp",
                    "with_ssl", "with_libsvtav1", "with_libaom",
                    "with_libdav1d", "with_xlib", "with_vaapi", "with_vdpau",
                    "with_vulkan", "with_libalsa", "with_pulse",
                    "with_appkit", "with_avfoundation", "with_coreimage",
                    "with_audiotoolbox", "with_videotoolbox"):
            try:
                setattr(ff, opt, False)
            except Exception:
                pass  # not every option exists on every platform's recipe

    def layout(self):
        cmake_layout(self)
