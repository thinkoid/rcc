// -*- mode: c++; -*-

// The driver: rcc's command line, and the processes it runs.  clang
// preprocesses the input before the front end.  After it, llc turns
// the IR the front end wrote into an object, instruction for
// instruction, and clang links the object.  Nothing optimizes on the
// way: the code that comes out is the code that was written.  The
// processes are spawned by fork and exec, never through a shell, so
// no argument is reinterpreted on the way.  With -native, rcc's own
// code generator writes assembly in place of the IR, and clang
// assembles it and links; no llc runs.

#include "c1.hh"
#include "emit.hh"
#include "error.hh"
#include "lex.hh"
#include "parse.hh"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace rcc {

namespace {

// The programs rcc runs, found on PATH.
constexpr const char *clang = "clang";
constexpr const char *llc = "llc";

// The flags llc runs with.  FastISel selects each IR instruction on
// its own, the real register allocator assigns the registers, and
// every machine pass that would rewrite, move, merge or drop an
// instruction is off.  Most are hidden options, not a stable
// interface: the set is verified on LLVM 23.1.3, and a later llc that
// renames or drops one refuses it as an unknown option.
const char *const llc_flags[] = {
    "-O1",                              // the real register allocator; -O0
                                        // allows only the fast one, which
                                        // spills across every block
    "-fast-isel",                       // the instruction written is the one
                                        // selected; no folding of a + 0,
                                        // x * 3, x / 4
    "-fast-isel-abort=3",               // never fall back to the folding
                                        // selector; fail loudly instead
    "-relocation-model=pic",            // clang links position-independent
                                        // executables
    "-disable-cgp",                     // no IR rewriting before selection
    "-disable-lsr",                     // no loop strength reduction
    "-disable-constant-hoisting",       // constants stay where written
    "-disable-machine-licm",            // nothing moves out of a loop
    "-disable-machine-cse",             // nothing written twice is computed
                                        // once
    "-disable-machine-sink",            // nothing moves down a branch
    "-disable-postra-machine-sink",     // nor after register allocation
    "-disable-machine-dce",             // FastISel already drops an unused
                                        // result; this covers the rest
    "-disable-peephole",                // no instruction rewriting
    "-disable-x86-lea-opt",             // no lea rewriting
    "-disable-x86-domain-reassignment", // no domain changes
    "-disable-early-ifcvt",             // no branch to conditional move
    "-x86-cmov-converter=false",        // no conditional move to branch
    "-enable-misched=false",            // no reordering
    "-disable-copyprop",                // no copy propagation
    "-disable-branch-fold",             // branches stay as written
    "-disable-tail-duplicate",          // no block duplication
    "-disable-early-taildup",           // nor early
    "-disable-block-placement",         // blocks stay in the written order
    "-disable-post-ra",                 // no post-allocation scheduling
    "-no-stack-coloring",               // one slot per local, never shared
};

constexpr const char *usage =
    "usage: rcc [-native] [-w] [-S|-c] [-o out] file.c";

// The last thing the driver makes: the IR, an object, or an executable.
// Under -native, the first is the assembly.
enum class stage_t { ir, object, executable };

struct options_t
{
    stage_t stage = stage_t::executable;
    bool native = false;
    bool warnings = true;
    std::string out;
    std::string file;
};

// What a child wrote to its standard output, and how it ended.
struct captured_t
{
    int status;
    std::string text;
};

// A failed system call, as the exception main reports.  The errno is
// taken by the caller before anything that could overwrite it.
[[noreturn]] void
throw_system_error(int err, std::string_view what)
{
    throw std::system_error(err, std::generic_category(), std::string(what));
}

// cc's defaults: the IR, the assembly and the object are named after
// the input and land in the current directory; the executable is
// a.out.
std::string
default_output(const options_t &opts)
{
    if (opts.stage == stage_t::executable)
        return "a.out";

    const auto stem = std::filesystem::path(opts.file).stem().string();

    if (opts.stage == stage_t::object)
        return stem + ".o";

    return stem + (opts.native ? ".s" : ".ll");
}

// The command line, or nothing when it is a usage error: an unknown
// option, -O<n> among them, since there are no optimization levels;
// -o without a name; -S with -c; or other than one input file.  A
// repeated -o takes the last one, as cc does.
std::optional< options_t >
options_from(int argc, char **argv)
{
    options_t opts;
    bool to_ir = false, to_object = false;

    for (int i = 1; i < argc; ++i) {
        const std::string_view arg = argv[i];

        if (arg == "-S")
            to_ir = true;
        else if (arg == "-native")
            opts.native = true;
        else if (arg == "-w")
            opts.warnings = false;
        else if (arg == "-c")
            to_object = true;
        else if (arg == "-o" && i + 1 < argc)
            opts.out = argv[++i];
        else if (arg.starts_with('-') || !opts.file.empty())
            return std::nullopt;
        else
            opts.file = arg;
    }

    if (opts.file.empty() || (to_ir && to_object))
        return std::nullopt;

    if (to_ir)
        opts.stage = stage_t::ir;
    else if (to_object)
        opts.stage = stage_t::object;

    if (opts.out.empty())
        opts.out = default_output(opts);

    return opts;
}

// The argument vector execvp takes, pointing into the strings and
// ending in a null.  Built before the fork, so the child only execs.
std::vector< char * >
argv_of(const std::vector< std::string > &args)
{
    std::vector< char * > argv;
    for (const auto &arg : args)
        argv.push_back(const_cast< char * >(arg.c_str()));
    argv.push_back(nullptr);

    return argv;
}

// In the child: says what failed and leaves with 127, the status a
// shell gives a command it could not run.  _exit, so the buffers the
// child inherited are not flushed a second time.
[[noreturn]] void
die_in_child(const char *what)
{
    std::cerr << "rcc: " << what << ": " << std::strerror(errno) << '\n';
    ::_exit(127);
}

// Replaces the child with the program; returns only through
// die_in_child, when the program could not be run.
[[noreturn]] void
exec_child(const std::vector< char * > &argv)
{
    ::execvp(argv[0], argv.data());
    die_in_child(argv[0]);
}

// The exit status as a shell reports it: the program's own, or 128
// plus the signal that ended it.
int
status_of(int wstatus)
{
    if (WIFSIGNALED(wstatus))
        return 128 + WTERMSIG(wstatus);

    return WEXITSTATUS(wstatus);
}

// Waits for the child, through any signal that interrupts the wait;
// returns its exit status.
int
wait_for(pid_t pid)
{
    int wstatus = 0;
    while (::waitpid(pid, &wstatus, 0) < 0)
        if (errno != EINTR)
            throw_system_error(errno, "waitpid");

    return status_of(wstatus);
}

// Reads the descriptor to end of file onto text; returns 0, or the
// errno of the read that failed.
int
read_all(int fd, std::string &text)
{
    char buf[65536];

    for (;;) {
        const ssize_t n = ::read(fd, buf, sizeof buf);
        if (n > 0)
            text.append(buf, n);
        else if (n == 0)
            return 0;
        else if (errno != EINTR)
            return errno;
    }
}

// Writes all of text and closes the descriptor; returns 0, or the
// errno of the first call that failed.  The close is checked: a file
// system may report a failed write only there.
int
write_and_close(int fd, std::string_view text)
{
    int err = 0;

    while (!text.empty() && err == 0) {
        const ssize_t n = ::write(fd, text.data(), text.size());
        if (n >= 0)
            text.remove_prefix(n);
        else if (errno != EINTR)
            err = errno;
    }

    if (::close(fd) != 0 && err == 0)
        err = errno;

    return err;
}

// Runs a program with its standard output read back through a pipe;
// its standard error stays rcc's, so its diagnostics reach the user
// unchanged.  The output is read to the end before the wait, so a
// child that fills the pipe cannot block forever.
captured_t
capture(const std::vector< std::string > &args)
{
    const auto argv = argv_of(args);

    int fds[2];
    if (::pipe(fds) != 0)
        throw_system_error(errno, "pipe");

    const pid_t pid = ::fork();
    if (pid < 0) {
        const int err = errno;
        ::close(fds[0]);
        ::close(fds[1]);
        throw_system_error(err, "fork");
    }

    if (pid == 0) {
        if (::dup2(fds[1], STDOUT_FILENO) < 0)
            die_in_child("dup2");
        ::close(fds[0]);
        ::close(fds[1]);
        exec_child(argv);
    }

    ::close(fds[1]);

    captured_t result { 0, { } };
    const int err = read_all(fds[0], result.text);
    ::close(fds[0]);

    result.status = wait_for(pid);
    if (err != 0)
        throw_system_error(err, "cannot read the output of " + args[0]);

    return result;
}

// Runs a program to completion on rcc's own standard streams; returns
// its exit status.
int
run(const std::vector< std::string > &args)
{
    const auto argv = argv_of(args);

    const pid_t pid = ::fork();
    if (pid < 0)
        throw_system_error(errno, "fork");

    if (pid == 0)
        exec_child(argv);

    return wait_for(pid);
}

// Writes the IR where -S asked for it, replacing any file there.
void
write_file(const std::string &path, std::string_view text)
{
    const int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    const int err = fd < 0 ? errno : write_and_close(fd, text);
    if (err != 0)
        throw_system_error(err, "cannot write " + path);
}

// The name mkstemps fills in: under $TMPDIR when it is set, /tmp
// otherwise.  The suffix keeps the file's kind visible: .ll for the
// IR llc reads, .o for the object clang links, which clang takes for
// linker input by that suffix, .s for the assembly clang assembles.
std::string
temp_template(std::string_view suffix)
{
    const char *dir = std::getenv("TMPDIR");
    if (dir == nullptr || *dir == '\0')
        dir = "/tmp";

    return std::string(dir) + "/rcc-XXXXXX" + std::string(suffix);
}

// A temporary file: the IR for llc, the empty place llc writes the
// object into for clang to link, or the assembly for clang.  It is
// removed when the object dies, on every path out of the scope that
// made it, exceptions included.
class temp_file_t
{
public:

    explicit temp_file_t(std::string_view suffix, std::string_view text = { })
        : m_path(temp_template(suffix))
    {
        const int fd = ::mkstemps(m_path.data(), suffix.size());
        if (fd < 0)
            throw_system_error(errno, "cannot create a temporary file");

        const int err = write_and_close(fd, text);
        if (err != 0) {
            ::unlink(m_path.c_str());
            throw_system_error(err, "cannot write " + m_path);
        }
    }

    ~temp_file_t()
    {
        ::unlink(m_path.c_str());
    }

    temp_file_t(const temp_file_t &) = delete;
    temp_file_t &operator=(const temp_file_t &) = delete;

    const std::string &path() const
    {
        return m_path;
    }

private:

    std::string m_path;
};

// A diagnostic in cc's format, an error or a warning.
void
report(const location_t &where, std::string_view kind, std::string_view what)
{
    std::cerr << where.file << ':' << where.line << ':' << where.column << ": "
              << kind << ": " << what << '\n';
}

void
report(const error_t &e)
{
    report(e.where, "error", e.what);
}

// Runs llc on the IR file with the literal flag set, writing the
// object; returns llc's exit status.  The IR names no target triple,
// so llc selects for its default, the host.
int
write_object(const std::string &ll, const std::string &obj)
{
    std::vector< std::string > args = { llc };
    for (const char *flag : llc_flags)
        args.push_back(flag);
    args.insert(args.end(), { "-filetype=obj", "-o", obj, ll });

    return run(args);
}

// Hands the IR to llc to make the object and, without -c, the object
// to clang to link; returns the status of llc when it failed or made
// the output, clang's otherwise.  clang only links: the object is
// final, and nothing it is told could change an instruction.
int
assemble(const options_t &opts, std::string_view ir)
{
    const temp_file_t ll(".ll", ir);

    if (opts.stage == stage_t::object)
        return write_object(ll.path(), opts.out);

    const temp_file_t obj(".o");

    const int status = write_object(ll.path(), obj.path());
    if (status != 0)
        return status;

    return run({ clang, obj.path(), "-o", opts.out });
}

// Hands the assembly to clang to assemble into the object and, without
// -c, to link; returns clang's exit status.
int
assemble_native(const options_t &opts, std::string_view text)
{
    const temp_file_t s(".s", text);

    if (opts.stage == stage_t::object)
        return run({ clang, "-c", s.path(), "-o", opts.out });

    return run({ clang, s.path(), "-o", opts.out });
}

// The pipeline: preprocess, tokenize, parse, emit, then the IR file or
// llc and clang; under -native, generate, then the assembly file or
// clang.  Returns rcc's exit status, which no warning changes; -w
// silences the warnings.  A diagnostic names its file by a view into
// the preprocessed text, so it is reported in this frame, while the
// text is alive.
int
drive(const options_t &opts)
{
    const auto pp = capture({ clang, "-E", opts.file });
    if (pp.status != 0)
        return pp.status;

    std::ostringstream ir;
    try {
        const auto tokens = tokenize(pp.text, opts.file);
        const auto unit = parse(tokens);
        if (opts.warnings)
            for (const auto &w : unit.warnings)
                report(w.where, "warning", w.what);

        if (opts.native)
            generate(unit, opts.file, ir);
        else
            emit(unit, ir);
    }
    catch (const error_t &e) {
        report(e);
        return 1;
    }

    if (opts.stage == stage_t::ir) {
        write_file(opts.out, ir.str());
        return 0;
    }

    if (opts.native)
        return assemble_native(opts, ir.str());

    return assemble(opts, ir.str());
}

} // namespace

} // namespace rcc

// Exit status: clang's or llc's own when it failed or made the output;
// 1 for an error in the source or a failed system call; 2 for a usage
// error; 3 for an internal error, which includes a construct the front
// end does not handle yet.
int
main(int argc, char **argv)
{
    const auto opts = rcc::options_from(argc, argv);
    if (!opts) {
        std::cerr << rcc::usage << '\n';
        return 2;
    }

    try {
        return rcc::drive(*opts);
    }
    catch (const std::system_error &e) {
        std::cerr << "rcc: error: " << e.what() << '\n';
        return 1;
    }
    catch (const std::exception &e) {
        std::cerr << "rcc: internal error: " << e.what() << '\n';
        return 3;
    }
}
