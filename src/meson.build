
waypipe_source_files = ['bench.c', 'client.c', 'dmabuf.c', 'handlers.c', 'kernel.c', 'mainloop.c', 'parsing.c', 'server.c', 'shadow.c', 'interval.c', 'util.c', 'video.c']
waypipe_dependencies = [
	libgbm,          # General GPU buffer creation, aligned with dmabuf proto
	liblz4,          # Fast compression option
	libzstd,         # Slow compression option
	libavcodec,libavutil,libswscale, # Video encoding
	pthreads,        # To run expensive computations in parallel
	protos,          # Wayland protocol data
	rt,              # For shared memory
	libva,           # For NV12->RGB conversions
]

# Conditionally compile SIMD-optimized code.
# (The meson simd module is a bit too limited for this)
kernel_libs = []
if cc.has_argument('-mavx512f') and cc.has_argument('-mlzcnt') and cc.has_argument('-mbmi')
	kernel_libs += static_library('kernel_avx512f', 'kernel_avx512f.c', c_args:['-mavx512f', '-mlzcnt', '-mbmi'])
	config_data.set('HAVE_AVX512F', 1, description: 'Compiler supports AVX-512F')
endif
if cc.has_argument('-mavx2') and cc.has_argument('-mlzcnt') and cc.has_argument('-mbmi')
	kernel_libs += static_library('kernel_avx2', 'kernel_avx2.c', c_args:['-mavx2', '-mlzcnt', '-mbmi'])
	config_data.set('HAVE_AVX2', 1, description: 'Compiler supports AVX2')
endif
if cc.has_argument('-msse4.1')
	kernel_libs += static_library('kernel_sse41', 'kernel_sse41.c', c_args:['-msse4.1'])
	config_data.set('HAVE_SSE41', 1, description: 'Compiler supports SSE 4.1')
endif
if host_machine.cpu_family() == 'aarch64' or cc.has_argument('-mfpu=neon')
	neon_args = host_machine.cpu_family() == 'aarch64' ? [] : ['-mfpu=neon']

	# Clang additionally enforces that NEON code only be compiled
	# to target a CPU that actually supports NEON instructions,
	# so bump the host CPU version for the optionally executed code only.
	if host_machine.cpu_family() == 'arm' and cc.get_id() == 'clang'
		host_cpu = host_machine.cpu()
		if host_cpu.contains('4') or host_cpu.contains('5') or host_cpu.contains('6')
			neon_args += ['-march=armv7-a']
		endif
	endif

	kernel_libs += static_library('kernel_neon', 'kernel_neon.c', c_args:neon_args)
	config_data.set('HAVE_NEON', 1, description: 'Compiler supports NEON')
endif

configure_file(
	output: 'config-waypipe.h',
	configuration: config_data,
)

lib_waypipe_src = static_library(
	'waypipe_src',
	waypipe_source_files,
	include_directories: waypipe_includes,
	link_with: kernel_libs,
	dependencies: waypipe_dependencies,
)

waypipe_prog = executable(
	'waypipe',
	['waypipe.c'],
	link_with: lib_waypipe_src,
	install: true
)