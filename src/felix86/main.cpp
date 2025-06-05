#include <csetjmp>
#include <fstream>
#include <argp.h>
#include <dirent.h>
#include <fcntl.h>
#include <fmt/format.h>
#include <grp.h>
#include <spawn.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Zydis/DecoderTypes.h"
#include "Zydis/Disassembler.h"
#include "biscuit/cpuinfo.hpp"
#include "felix86/common/config.hpp"
#include "felix86/common/info.hpp"
#include "felix86/common/log.hpp"
#include "felix86/common/sudo.hpp"
#include "felix86/common/symlink.hpp"
#include "felix86/common/utility.hpp"
#include "felix86/emulator.hpp"
#include "felix86/hle/signals.hpp"
#include "felix86/hle/thunks.hpp"

#if !defined(__riscv)
#pragma message("You are compiling for x86-64, felix86 should only be compiled for RISC-V, are you sure you want to do this?")
#endif

std::string version_full = get_version_full();
const char* argp_program_version = version_full.c_str();
const char* argp_program_bug_address = "<https://github.com/OFFTKP/felix86/issues>";

static char doc[] = "felix86 - a userspace x86_64 emulator";
static char args_doc[] = "TARGET_BINARY [TARGET_ARGS...]";

static struct argp_option options[] = {
    {"info", 'i', 0, 0, "Print system info"},
    {"configs", 'c', 0, 0, "Print the emulator configurations"},
    {"kill-all", 'k', 0, 0, "Kill all open emulator instances"},
    {"set-rootfs", 's', "DIR", 0, "Set the rootfs path in config.toml"},
    {"set-thunks", 'S', "DIR", 0, "Set the thunks path in config.toml"},
    {"binfmt-misc", 'b', 0, 0, "Register the emulator in binfmt_misc so that x86-64 executables can run without prepending the emulator path"},
    {"unregister-binfmt-misc", 'u', 0, 0, "Unregister the emulator from binfmt_misc"},
    {0}};

int guest_arg_start_index = -1;

template <>
struct fmt::formatter<std::filesystem::path> : formatter<std::string_view> {
    template <typename FormatContext>
    auto format(const std::filesystem::path& path, FormatContext& ctx) const {
        return formatter<std::string_view>::format(path.string(), ctx);
    }
};

int print_system_info() {
    printf("%s\n", version_full.c_str());

    using namespace biscuit;
    biscuit::CPUInfo info;
    bool V = info.Has(Extension::V);
    int len = 0;
    if (V) {
        len = info.GetVlenb();
        printf("VLEN: %d\n", len * 8);
    }

    fflush(stdout);

    std::vector<const char*> args = {"neofetch", "cpu", nullptr};

    pid_t pid;
    int status;
    int ok = posix_spawnp(&pid, "neofetch", nullptr, nullptr, (char**)args.data(), environ);
    if (ok != 0)
        goto error;
    waitpid(pid, &status, 0);

    args[1] = "gpu";
    ok = posix_spawnp(&pid, "neofetch", nullptr, nullptr, (char**)args.data(), environ);
    if (ok != 0)
        goto error;
    waitpid(pid, &status, 0);

    args[1] = "model";
    ok = posix_spawnp(&pid, "neofetch", nullptr, nullptr, (char**)args.data(), environ);
    if (ok != 0)
        goto error;
    waitpid(pid, &status, 0);

    args[1] = "distro";
    ok = posix_spawnp(&pid, "neofetch", nullptr, nullptr, (char**)args.data(), environ);
    if (ok != 0)
        goto error;
    waitpid(pid, &status, 0);

    args[1] = "de";
    ok = posix_spawnp(&pid, "neofetch", nullptr, nullptr, (char**)args.data(), environ);
    if (ok != 0)
        goto error;
    waitpid(pid, &status, 0);

    args[1] = "wm";
    ok = posix_spawnp(&pid, "neofetch", nullptr, nullptr, (char**)args.data(), environ);
    if (ok != 0)
        goto error;
    waitpid(pid, &status, 0);

    args[1] = "kernel";
    ok = posix_spawnp(&pid, "neofetch", nullptr, nullptr, (char**)args.data(), environ);
    if (ok != 0)
        goto error;
    waitpid(pid, &status, 0);

    args[1] = "memory";
    ok = posix_spawnp(&pid, "neofetch", nullptr, nullptr, (char**)args.data(), environ);
    if (ok != 0)
        goto error;
    waitpid(pid, &status, 0);

    return 0;

error:
    printf("Please install neofetch for more information\n");
    return ok;
}

bool detect_binfmt_misc() {
    // Run an x86-64 program and set __FELIX86_TEST_BINFMT_MISC which will make felix86 immediately return 0x42.
    // If the return value is 0x42 that means felix86 was invoked and thus binfmt_misc is correctly installed.
    // If anything else is returned it means we didn't run it through binfmt_misc thus it's not installed.
    std::error_code ec;
    std::filesystem::path path = g_config.rootfs_path / "bin/env";
    if (std::filesystem::exists(path, ec) && std::filesystem::is_regular_file(path, ec)) {
        pid_t pid;
        int status;

        std::vector<char*> envs;

        char** env = environ;
        while (*env) {
            envs.push_back(*env);
            env++;
        }

        char buf[256] = "__FELIX86_TEST_BINFMT_MISC=1";
        envs.push_back(buf);
        envs.push_back(nullptr);

        std::vector<const char*> args = {
            path.c_str(),
            nullptr,
        };

        int devnull = open("/dev/null", O_WRONLY);
        if (devnull == -1) {
            return false;
        }

        posix_spawn_file_actions_t actions;
        posix_spawn_file_actions_init(&actions);
        posix_spawn_file_actions_adddup2(&actions, devnull, STDOUT_FILENO);
        posix_spawn_file_actions_adddup2(&actions, devnull, STDERR_FILENO);

        if (posix_spawn(&pid, path.c_str(), &actions, NULL, (char**)args.data(), (char**)envs.data()) != 0) {
            WARN("posix_spawn failed: %d", errno);
            return false;
        }

        int exit_status = 0;
        if (waitpid(pid, &status, 0) == -1) {
            exit_status = -1;
        } else {
            if (WIFEXITED(status)) {
                exit_status = WEXITSTATUS(status);
            } else {
                exit_status = -1;
            }
        }

        close(devnull);
        posix_spawn_file_actions_destroy(&actions);

        // $ROOTFS/bin/env was run through felix86, thus binfmt_misc is installed
        return exit_status == 0x42;
    } else {
        WARN("rootfs/bin/env not found?");
        return false;
    }
}

// TODO: Move me to binfmt.hpp file along with unregister_binfmt_misc
void binfmt_misc(bool is_register) {
    if (!Sudo::hasPermissions()) {
        printf("I need root permissions to register/unregister felix86 in binfmt_misc, please re-run with root permissions\n");
        exit(1);
    }

    Config::initialize();
    if (g_config.rootfs_path.empty()) {
        printf("Rootfs path is not set, did you not pass the environment variables when running with sudo? Try `sudo -E felix86 --binfmt-misc` or "
               "set the rootfs path\n");
        exit(1);
    }

    char exe_path[4096];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        perror("readlink");
    }
    exe_path[len] = '\0';

    std::string registration_string_x64 = fmt::format(
        R"!(:felix86-x86_64:M:0:\x7fELF\x02\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x3e\x00:\xff\xff\xff\xff\xff\xfe\xfe\x00\x00\x00\x00\xff\xff\xff\xff\xff\xfe\xff\xff\xff:{}:OCF)!",
        exe_path);
    std::string registration_string_i386 = fmt::format(
        R"!(:felix86-i386:M:0:\x7fELF\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x03\x00:\xff\xff\xff\xff\xff\xfe\xfe\x00\x00\x00\x00\xff\xff\xff\xff\xff\xfe\xff\xff\xff:{}:OCF)!",
        exe_path);

    if (!is_register) {
        if (unregister_binfmt_misc("felix86-x86_64")) {
            printf("Unregistered felix86 from binfmt_misc for x86-64 apps\n");
        }

        if (unregister_binfmt_misc("felix86-i386")) {
            printf("Unregistered felix86 from binfmt_misc for i386 apps\n");
        }

        g_config.binfmt_misc_installed = false;
        Config::save(g_config.path(), g_config);
        printf("felix86 successfully unregistered from binfmt_misc\n");
    } else {
        // Unregister if already registered
        unregister_binfmt_misc("felix86-x86_64");
        unregister_binfmt_misc("felix86-i386");

        FILE* fp = fopen("/proc/sys/fs/binfmt_misc/register", "w");

        if (!fp) {
            ERROR("Failed to open /proc/sys/fs/binfmt_misc/register");
        }

        if (fwrite(registration_string_x64.c_str(), 1, registration_string_x64.size(), fp) != registration_string_x64.size()) {
            fclose(fp);
            ERROR("Failed to register for x86-64");
        }

        fclose(fp);

        fp = fopen("/proc/sys/fs/binfmt_misc/register", "w");

        if (fwrite(registration_string_i386.c_str(), 1, registration_string_i386.size(), fp) != registration_string_i386.size()) {
            fclose(fp);
            ERROR("Failed to register for i386");
        }

        fclose(fp);

        // /proc/sys/fs stuff makes it temporary, we need to install it here to make it permanent via systemd
        // Register it in the first available directory in this order
        std::vector<std::filesystem::path> dirs = {
            "/etc/binfmt.d",
            "/usr/lib/binfmt.d",
            "/usr/local/lib/binfmt.d",
            "/run/binfmt.d",
        };

        bool registered = false;
        for (auto& dir : dirs) {
            if (std::filesystem::exists(dir)) {
                {
                    std::filesystem::path x64path = dir / "felix86-x86_64.conf";
                    FILE* fp = fopen(x64path.c_str(), "w");
                    if (!fp) {
                        ERROR("Failed to open %s", x64path.c_str());
                    }
                    if (fwrite(registration_string_x64.c_str(), 1, registration_string_x64.size(), fp) != registration_string_x64.size()) {
                        fclose(fp);
                        ERROR("Failed to register in binfmt.d for x86-64");
                    }
                    fclose(fp);
                }
                {
                    std::filesystem::path i386path = dir / "felix86-i386.conf";
                    FILE* fp = fopen(i386path.c_str(), "w");
                    if (!fp) {
                        ERROR("Failed to open %s", i386path.c_str());
                    }
                    if (fwrite(registration_string_i386.c_str(), 1, registration_string_i386.size(), fp) != registration_string_i386.size()) {
                        fclose(fp);
                        ERROR("Failed to register in binfmt.d for i386");
                    }
                    fclose(fp);
                }

                registered = true;
                break;
            }
        }

        if (!registered) {
            printf("Failed to find a binfmt.d directory to put felix86.conf in\n");
        }

        unregister_binfmt_misc("qemu-x86_64");
        unregister_binfmt_misc("qemu-i386");

        g_config.binfmt_misc_installed = true;
        Config::save(g_config.path(), g_config);

        if (!detect_binfmt_misc()) {
            printf(ANSI_COLOR_YELLOW
                   "Even though I installed felix86 in binfmt_misc, I couldn't run a simple binary with it. Either /bin/env is missing in rootfs or "
                   "there's conflicting emulators in binfmt_misc which may make felix86 not work correctly" ANSI_COLOR_RESET "\n");
        }

        printf("felix86 successfully registered to binfmt_misc\n");
    }
}

void kill_all() {
    DIR* proc_dir;
    struct dirent* entry;
    pid_t self_pid = getpid();

    proc_dir = opendir("/proc");
    if (!proc_dir) {
        perror("opendir /proc");
    }

    std::string our_name;
    char self[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", self, PATH_MAX - 1);
    if (len == -1) {
        printf("Failed to read /proc/self/exe? Using `felix86` as search name");
        our_name = "felix86";
    } else {
        self[len] = 0;
        our_name = basename(self);
    }

    while ((entry = readdir(proc_dir)) != NULL) {
        pid_t pid = atoi(entry->d_name);
        if (pid == self_pid)
            continue;

        if (pid == 0)
            continue;

        char exe_path[PATH_MAX];
        snprintf(exe_path, sizeof(exe_path), "/proc/%d/exe", pid);

        char exe_target[PATH_MAX];
        ssize_t len = readlink(exe_path, exe_target, PATH_MAX - 1);
        if (len == -1)
            continue;

        exe_target[len] = '\0';

        std::string path = exe_target;
        if (path.find(' ') != std::string::npos) {
            // Sometimes paths come up as "/path/to/felix86 (deleted)"
            // This happens when the executable... was deleted
            // Helpful when I eg. recompile but also wanna kill old running instances
            // ie. wine leftovers and stuff like that
            path = path.substr(0, path.find(' '));
        }

        char* base = basename(path.data());

        if (strcmp(base, our_name.c_str()) == 0) {
            if (kill(pid, SIGKILL) == 0) {
                printf("Killed process %d\n", pid);
            } else {
                printf("Failed to kill process %d", pid);
            }
        }
    }

    closedir(proc_dir);
}

static error_t parse_opt(int key, char* arg, struct argp_state* state) {
    StartParameters* params = (StartParameters*)state->input;

    if (key == ARGP_KEY_ARG) {
        if (params->argv.empty()) {
            params->executable_path = arg;
        }

        params->argv.push_back(arg);
        guest_arg_start_index = state->next;
        state->next = state->argc; // tell argp to stop
        return 0;
    }

    switch (key) {
    case 'i': {
        exit(print_system_info());
        break;
    }
    case 'k': {
        kill_all();
        exit(0);
        break;
    }
    case 'b': {
        binfmt_misc(true);
        exit(0);
        break;
    }
    case 'u': {
        binfmt_misc(false);
        exit(0);
        break;
    }
    case 's': {
        Config::initialize();
        char* real_path = realpath(arg, nullptr);
        if (!real_path) {
            printf("Could not resolve %s to an absolute path", arg);
            exit(1);
        }
        printf("Setting rootfs to %s\n", real_path);
        g_config.rootfs_path = real_path;
        Config::save(g_config.path(), g_config);
        exit(0);
        break;
    }
    case 'S': {
        Config::initialize();
        char* real_path = realpath(arg, nullptr);
        if (!real_path) {
            printf("Could not resolve %s to an absolute path", arg);
            exit(1);
        }
        printf("Setting thunks path to %s\n", real_path);
        g_config.thunks_path = real_path;
        Config::save(g_config.path(), g_config);
        exit(0);
        break;
    }
    case 'c': {
        // TODO: add some color here
        Config::initialize();

        std::string current_group;
        printf("These are the configurations for felix86\n");
        printf("You may edit %s or set the corresponding environment variable\n", g_config.path().c_str());

#define X(group, type, name, def, env, description, required)                                                                                        \
    if (current_group != #group) {                                                                                                                   \
        current_group = #group;                                                                                                                      \
        printf("\n[%s]\n", current_group.c_str());                                                                                                   \
    }                                                                                                                                                \
    fmt::print("{} {} = {} (default: {}) -- Environment variable: {}\n", #type, #name, g_config.name, #def, #env);
#include "felix86/common/config.inc"
#undef X
        exit(0);
        break;
    }
    case ARGP_KEY_END: {
        if (params->argv.empty()) {
            argp_usage(state);
        }
        break;
    }

    default: {
        return ARGP_ERR_UNKNOWN;
    }
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char* argv[]) {
    if (getenv("__FELIX86_TEST_BINFMT_MISC")) {
        // This shouldn't be printed as when we run /bin/env in detect_binfmt_misc we mute stdout and stderr
        WARN("__FELIX86_TEST_BINFMT_MISC was detected, if you see this then something is wrong");

        // Magic value expected by detect_binfmt_misc
        return 0x42;
    }

#ifdef __x86_64__
    WARN("You're running an x86-64 executable version of felix86, get ready for a crash soon");
#endif
    StartParameters params = {};

    argp_parse(&argp, argc, argv, ARGP_IN_ORDER, 0, &params);
    if (guest_arg_start_index != -1) {
        char** argv_next = &argv[guest_arg_start_index];
        while (*argv_next) {
            params.argv.push_back(*argv_next);
            argv_next++;
        }
    }

    g_execve_process = !!getenv("__FELIX86_EXECVE");
    if (!g_execve_process) {
        Logger::startServer();
    } else {
        Logger::joinServer();
    }

    Config::initialize();
    initialize_globals();

    std::filesystem::path xauthority_path;
    const char* xauth_env = getenv("XAUTHORITY");
    if (xauth_env) {
        xauthority_path = xauth_env;
    } else {
        // Also check $HOME
        const char* home_env = getenv("HOME");
        if (home_env) {
            xauthority_path = std::filesystem::path(home_env) / ".Xauthority";
        } else {
            WARN("Couldn't find $HOME");
        }
    }

    if (!g_execve_process) {
        ASSERT(!g_config.rootfs_path.empty());

        // First time running the emulator (ie. the emulator is not running itself with execve) we need to link some stuff
        // and copy some stuff inside the rootfs
        auto copy = [](const char* src, const std::filesystem::path& dst) {
            if (!std::filesystem::exists(src)) {
                printf("I couldn't find %s to copy to the rootfs, may cause problems with some games\n", src);
                return;
            }

            using co = std::filesystem::copy_options;

            std::error_code ec;
            std::filesystem::copy(src, dst, co::overwrite_existing | co::recursive, ec);
            if (ec) {
                VERBOSE("Error while copying %s: %s", src, ec.message().c_str());
            }
        };

        std::filesystem::create_directories(g_config.rootfs_path / "var" / "lib");
        std::filesystem::create_directories(g_config.rootfs_path / "etc");

        // Copy some stuff to the g_config.rootfs_path
        copy("/var/lib/dbus", g_config.rootfs_path / "var" / "lib" / "dbus");
        copy("/etc/mtab", g_config.rootfs_path / "etc" / "mtab");
        copy("/etc/passwd", g_config.rootfs_path / "etc" / "passwd");
        copy("/etc/passwd-", g_config.rootfs_path / "etc" / "passwd-");
        copy("/etc/group", g_config.rootfs_path / "etc" / "group");
        copy("/etc/group-", g_config.rootfs_path / "etc" / "group-");
        copy("/etc/shadow", g_config.rootfs_path / "etc" / "shadow");
        copy("/etc/shadow-", g_config.rootfs_path / "etc" / "shadow-");
        copy("/etc/gshadow", g_config.rootfs_path / "etc" / "gshadow");
        copy("/etc/gshadow-", g_config.rootfs_path / "etc" / "gshadow-");
        copy("/etc/hosts", g_config.rootfs_path / "etc" / "hosts");
        copy("/etc/hostname", g_config.rootfs_path / "etc" / "hostname");
        copy("/etc/timezone", g_config.rootfs_path / "etc" / "timezone");
        copy("/etc/localtime", g_config.rootfs_path / "etc" / "localtime");
        copy("/etc/fstab", g_config.rootfs_path / "etc" / "fstab");
        copy("/etc/subuid", g_config.rootfs_path / "etc" / "subuid");
        copy("/etc/subgid", g_config.rootfs_path / "etc" / "subgid");
        copy("/etc/machine-id", g_config.rootfs_path / "etc" / "machine-id");
        copy("/etc/resolv.conf", g_config.rootfs_path / "etc" / "resolv.conf");

        // Symlink some directories to make our lives easier and not have to overlay them
        ASSERT_MSG(Symlinker::link("/run", g_config.rootfs_path / "run"), "Failed to symlink /run: %s", strerror(errno));
        ASSERT_MSG(Symlinker::link("/proc", g_config.rootfs_path / "proc"), "Failed to symlink /proc: %s", strerror(errno));
        ASSERT_MSG(Symlinker::link("/sys", g_config.rootfs_path / "sys"), "Failed to symlink /sys: %s", strerror(errno));
        ASSERT_MSG(Symlinker::link("/dev", g_config.rootfs_path / "dev"), "Failed to symlink /dev: %s", strerror(errno));

        mkdirat(g_rootfs_fd, "tmp", 0777);

        // Check if we can find the .Xauthority file inside the rootfs, otherwise warn
        // Since many distros put it in /run we should be able to
        if (!std::filesystem::exists(g_config.rootfs_path / xauthority_path.relative_path())) {
            WARN("I couldn't find the .Xauthority file inside the rootfs");
        }
    }

    Signals::initialize();

    if (is_subpath(params.argv[0], g_config.rootfs_path)) {
        params.argv[0] = params.argv[0].substr(g_config.rootfs_path.string().size());
        ASSERT(!params.argv[0].empty());
        if (params.argv[0].at(0) != '/') {
            params.argv[0] = '/' + params.argv[0];
        }
    }

    std::string args = "Arguments: ";
    for (const auto& arg : params.argv) {
        args += arg;
        args += " ";
    }
    VERBOSE("%s", args.c_str());

    if (g_execve_process) {
        const char* guest_envs = getenv("__FELIX86_GUEST_ENVS");
        if (guest_envs) {
            std::vector<std::string> envs = split_string(guest_envs, ',');
            for (auto& env : envs) {
                params.envp.push_back(env);
            }
        }
    } else {
        bool purposefully_empty = false;
        const char* env_file = getenv("FELIX86_ENV_FILE");
        if (env_file) {
            std::string env_path = env_file;
            if (std::filesystem::exists(env_path)) {
                std::ifstream env_stream(env_path);
                std::string line;
                while (std::getline(env_stream, line)) {
                    params.envp.push_back(line);
                }

                if (params.envp.empty()) {
                    purposefully_empty = true;
                }
            } else {
                WARN("Environment variable file %s does not exist. Using host environment variables.", env_file);
            }
        }

        if (params.envp.empty() && !purposefully_empty) {
            char** envp = environ;
            while (*envp) {
                params.envp.push_back(*envp);
                envp++;
            }
        }
    }

    // TODO: These "hacky" environment variables are bandaid solutions to problems that we need to eventually fix
    // They are enabled by default
    if (g_config.hacky_envs) {
        // Go uses a bunch of signals for preemption and this breaks our current signal handling
        // Apps like `snap` use go, and those are used sometimes by `apt`, and this async preemption is useless in a lot of programs
        params.envp.push_back("GODEBUG=asyncpreemptoff=1");

        // DOTNET tries to allocate too much heap memory, and many RISC-V boards currently come with 39-bit address space
        // To counteract this by default, we'll limit the heap memory dotnet allocates
        params.envp.push_back("DOTNET_GCHeapHardLimit=1C0000000");
    }

    auto it = params.envp.begin();
    while (it != params.envp.end()) {
        std::string env = *it;

        // Dont pass these to the executable itself
        if (env.find("FELIX86_") != std::string::npos) {
            it = params.envp.erase(it);
        } else {
            it++;
        }
    }

    // Resolve symlinks, get absolute path. If the symlink is resolved, it may not start with
    // the rootfs prefix, and we need to add it back
    const std::string rootfs_string = g_config.rootfs_path.string();
    std::filesystem::path resolved = Symlinker::resolve(params.executable_path);
    if (resolved.string().find(rootfs_string) != 0) {
        resolved = g_config.rootfs_path / resolved.relative_path();
    }
    params.executable_path = resolved;

    if (params.executable_path.empty()) {
        ERROR("Executable path not specified");
        return 1;
    } else {
        if (!std::filesystem::exists(params.executable_path)) {
            ERROR("Executable path does not exist: %s", params.executable_path.c_str());
            return 1;
        }

        if (!std::filesystem::is_regular_file(params.executable_path)) {
            ERROR("Executable path is not a regular file");
            return 1;
        }
    }

    if (!g_config.binfmt_misc_installed && !g_execve_process && check_if_privileged_executable(params.executable_path)) {
        // Privileged executable but no binfmt_misc support, warn the user
        WARN("This is a privileged executable but the emulator isn't installed in binfmt_misc, might run into problems. Run `felix86 -b` to install "
             "it, make sure to remove other x86/x86-64 emulators from binfmt_misc");
    }

    SIGLOG("New felix86 instance with PID %d and executable path %s", getpid(), params.executable_path.c_str());

    if (g_execve_process) {
        pthread_setname_np(pthread_self(), "ExecveProcess");
    } else {
        pthread_setname_np(pthread_self(), "MainProcess");
    }

    auto [exit_reason, exit_code] = Emulator::Start(params);

    if (!g_execve_process) {
        LOG("Main process %d exited with reason: %s. Exit code: %d", getpid(), print_exit_reason(exit_reason), exit_code);
    } else {
        LOG("Execve process %d exited with reason: %s. Exit code: %d", getpid(), print_exit_reason(exit_reason), exit_code);
    }

    return exit_code;
}
