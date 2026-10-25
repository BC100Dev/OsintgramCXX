# OsintgramCXX
```text
   ___     _       _                            
  /___\___(_)_ __ | |_ __ _ _ __ __ _ _ __ ___  
 //  // __| | '_ \| __/ _` | '__/ _` | '_ ` _ \ 
/ \_//\__ \ | | | | || (_| | | | (_| | | | | | |
\___/ |___/_|_| |_|\__\__, |_|  \__,_|_| |_| |_|
                      |___/                     
```

Ever since the development on [the original Project](https://github.com/Datalux/Osintgram)
originally stopped back in 2023, I decided to initiate a complete rewrite, with the original
attempt [being made in Java](https://github.com/BC100Dev/osintgram4j). Despite the original
project having resumed development on the web basis, I am tackling this rewrite anyway with
the original functionality of Osintgram v2.0 being implemented before HikerAPI was included
in the first place. This version of Osintgram, that being the CXX version, handles things
in the Terminal instead of on the web, persisting on the classic.

This means that if you primarily use tooling in the Terminal, this version is for you, when
it eventually stabilizes. On top of that, this version allows you to use the legacy version
of logging into your own Instagram accounts, with the strong recommendation being on the use
of alternative accounts.

---

## Use cases
As of now, there are no use cases yet. This project is still in active development, despite
the slow codebase updates. I will be conducting announcements and polls, in case of needing
to disclose more information or when planned additional features need voting first to
understand, what the OSINT community truly needs out of OsintgramCXX.

Alongside, people have been asking me about using this tool, with one clear example
[being shown here](https://github.com/BC100Dev/osintgram4j/issues/16) from the previous
version. I don't want to make this personal or offend anyone that are eagerly waiting for
this tool to come out, but actual development, with and without AI, does take time. This
means that actual development are not always "quick coding sessions", as you may imagine.
Development takes time, and so does thorough testing. Despite the time, I will be updating
on any updates.

However, what I can describe on what features are implemented (or will be) in here are these:
- **Modding Support**: Add your own Hooks and Commands to OsintgramCXX itself (requires C/C++
  coding skills)
- **DSI** (Device Spoofing Information): Spoofs a device for network calls to Instagram (what
  is an OSINT tool for the community without spoofing a device for the platform to be
  satisfied)
- **Manual Interaction**: Using this tool means that you have more control than having most
  actions automated (e.g. automated login)
- **Proxies & Proxychains**: Allows you to connect to multiple Proxies at once, allowing a
  connection through several servers
- **Multi-Session Interaction**: Target multiple profiles at once without having to automate
  more things
- **HikerAPI support**: With the HikerAPI being a better alternative to scraping user data on
  the Instagram platform, things become easier in obtaining the necessary data.
- **Legacy User Login support**: Most tools are phasing out the requirement of logging in.
  This one doesn't, keeping the classic ability of logging into your own account.

A note on the Legacy User Login:
> it is strongly recommended to no longer use it. Even if that part will still be maintained
> for those that want to use their own accounts, HikerAPI is strongly recommended. In the
> case of HikerAPI, DSI is not required. DSI is only strongly recommended, when you are using
> your own account as a way of logging in. Another thing to note is the fact that Instagram
> will most likely restrict your account, if they detect automation on your account. If the
> legacy user login is required, make sure to use an alternative account.

---

## Disclaimer & Privacy Policy
This project, OsintgramCXX, along with the original project, are merely intended for ethical
and legal use only. By the definition of OSINT, the use of this tool is to collect publicly
available information. The inclusions of obtaining information not meant to be shared
publicly are not being implemented for legal reasons. By default, Instagram does ***NOT***
expose sensitive information to the public, unless the person of interest on a particular
account has decided to make their details public, which may include, but not limited to:

- Public E-Mail addresses
- Business address
- Public phone number

Developers, myself included, do not support unethical activities like black-hat hacking
or gray-hat hacking, along wth other malicious activities. Always follow relevant laws of
your location and where you live. Despite what any other project may claim, development on
projects like these also make the developers held liable for the malicious actions taken by
a malicious user. The liabilities may include, but are not limited to, lawsuits, fines, cease
and desist letters, and potential jail time.

By using this tool, you acknowledge that:
- **Responsibility**: You are accountable for the actions you take
- **Ethical Use**: You do not misuse this tool to your advantage in harassing people
- **Actions getting logged**: Instagram will log each action you take using this tool
- **Liability**: we, the developers of Osintgram, are not liable for the consequences you 
  take by potentially misusing this tool.
- **Terms and Privacy Policy**: using this tool, you also break the Instagram's
  [ToS / Privacy Policy](https://help.instagram.com/581066165581870/?locale=en_US). As per the
  Privacy Policy violation, you may be in violation of other people's privacy, if you are
  doing activities using this tool without any consent.
- **Consensual Activities**: you are using this tool with the permission that you are
  targeting. Performing activities without consent or permission may lead to fines by law
  enforcement, especially towards people in or from the EU (GDPR fines), which may also lead
  to prison time.

---

## Build Process
First and foremost, clone the repository down. For this, you will need the `git` command.
If you don't have it at your disposal yet (e.g. "command not found"), install it first:

```shell
# Debian & Termux
$ sudo apt install git

# Arch Linux
$ sudo pacman -Sy git
```

For this, I recommend making a separate directory in your home directory to separate
downloaded projects via Git to not cluster your home directory. It's optional and this part
can be skipped entirely:

```shell
$ mkdir -p $HOME/Git
$ cd $HOME/Git
```

Once `git` is present on your system, clone the repository down using this command:

```shell
$ git clone https://github.com/BC100Dev/OsintgramCXX

# optionally, if you are using a forked version, replace the value of FORK_USER with
# the one that the fork is distributing, replacing BC100Dev in there with a different user.
export FORK_USER="BC100Dev"
$ git clone https://github.com/${FORK_USER}/OsintgramCXX OsintgramCXX-forked
```

After cloning down the repository, change your current working directory into OsintgramCXX:

```shell
$ cd OsintgramCXX

# again, if using a forked version
$ cd OsintgramCXX-forked
```

After having OsintgramCXX cloned down, you will need to prepare your system to be able to
build OsintgramCXX. Since this is a C++ version, it's not a straight "run it and perform",
meaning you have to build it first. To build the tool, you will need to install a few tools
that OsintgramCXX relies upon. On a build with `vcpkg`, in which the `prepare.sh` script
relies upon, it can automatically fetch the necessary dependencies for you, meaning you
don't need anything further other than CMake and the essential tools for building C/C++
projects.

You will need to use your system package manager. For this showcase, we will be using the
standard package manager of Debian and Termux, that being `apt`. `apt-get` also works too.
However, when you are using a different system, like Arch Linux, you're using `pacman`. The
same goes for Fedora / Red Hat systems, where you are using `rpm`. Commands and packages may
vary with your installed distribution.

When using the automatic `prepare.sh` script, you only need these packages installed, with
these being the only ones:

```shell
$ sudo apt install build-essential cmake tar unzip curl
```

However, when you are refusing to use `vcpkg` via the `prepare.sh` script, you will also
need these additional packages installed:

```shell
$ sudo apt install libssl-dev libcurl4-openssl-dev
```

After having the necessary packages and libraries installed, you can choose one of these
ways to build it.

### Using the automatic script (vcpkg)
For the automatic preparation, you need to perform these additional commands, which can
pull up `vcpkg` automatically, install the dependencies and prepare a CMake environment. Use
these commands to prepare a build environment:

```shell
# if you are not already in the OsintgramCXX root directory
$ cd OsintgramCXX

$ chmod +x ./prepare.sh
$ ./prepare.sh
```

However, if `vcpkg` is already installed, `vcpkg` will be updated and if packages are
missing or need to be updated, the script will follow suit.

In cases of cross-compilation, you can reuse the package management environment, as it will
be located under the current directory's path via `.vcpkg`. The script does not adhere for
cross-compilation, so that needs more manual work.

### Using in-system dependencies (manual)
Once you have the libraries installed, you can simply call onto the CMake directly to
prepare the environment.

```shell
$ cmake -S . -B Build -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

The profile `RelWithDebInfo` will make a release build with debug symbols, in case of
needing to debug certain things. It is helpful, when needing to trace things via a debugger
like `gdb`.

## Finalizing the build
Once a build environment has been prepared, use the `cmake` command to build OsintgramCXX.

```shell
$ cmake --build Build

# for faster builds
$ cmake --build Build --parallel
```

Upon the build being successful, the files should be located at `Build/bundle`, containing
the `OsintgramCXX` executable, alongside its needed libraries. Alongside the built files, a
directory by the name of `Resources` will also appear, as they are the core files for
OsintgramCXX to run properly without any issues. If an issue occurs because `Resources`
doesn't exist, or in worse case scenario, `commands.json` not being found or not loaded,
OsintgramCXX may not crash, but no commands will be present. The `AppSettings.cfg` file
is an optional file dedicated to on-startup settings.

However, if the build does fail, please report an issue by passing in the build logs of
what exactly failed and under what commit hash you are under. The commit hash is effective
due to the exact traceability of where and when exactly it has crashed. You may fetch out
the commit hash using either `git rev-parse HEAD` or via `echo $(git log -1 --format="%H")`.
The same goes for the linker, if it starts screaming about unresolved symbols, in which I
may or may not have forgotten to mention additional libraries that need to be installed.

In cases of needing to perform a complete rebuild, like additional headers or directory
inclusions not being populated right, you may have to perform this additional command to
start the build from start. This will only clean up certain cache and the build files that
have failed prior to the build.

```shell
$ cmake --build Build --target clean
```

In worse case scenario, you may have to perform this command, if the fixes have been applied
and the current build environment did not fix anything, forcing you to start from the start:

```shell
$ rm -rf Build
```

***!! For those already testing !!***  
Please note that this is still a WIP, and that this is not the finalized version. I will
make a release package for all the necessary targets, once OsintgramCXX is completed.

---

## Building on non-Linux systems
Okay, but you might be running Windows or macOS, so how else can you build it? Different
methods include:

- Virtual Machines
- Cloud Shell (via SSH)
- Containers (e.g. Docker)
- Dual-Booting (if you have a Windows PC)
- WSL (Windows only)
- Termux (Android only, still experimenting for a proper build)

A note on building OsintgramCXX directly from Termux: since it may or may not be a known
problem to some that are statically linking this project, you may encounter issues regarding
missing libraries. I am currently unaware of how to fix it, other than being able to use the
Android NDK version directly that Google provides via Android Studio. I will be tackling
the issues, once OsintgramCXX does eventually get its first release stage.

---

## Default modules
For the default installment of this tool, I decided to separate the codebase into multiple
sections, which include:

- [**Core Application**](Sources/Application): The initial module that fires up, once you
  execute `./OsintgramCXX` (if you are in the same directory as the build files, the `./`
  is the reason).
- [**Interactive Commands**](Sources/CoreCommands): The part that would give you the most
  interests, considering that it is the base module, where all the standard commands live at.
- [**Instagram Private API**](Sources/instagram-api): Oh, this one's spicy, eh? Yes,
  this code will be the main reason all your custom mods will most likely depend on. That is,
  if you don't rely on user input and probably automate.
- [**Development Tools**](Sources/DevTools): The Instagram APIs use them. The core
  application uses them. Most of the reusable code, whether that being the networking code,
  string utilities, checking for administrative permissions or doing other helpful things,
  this library contains most of the shared code.
- [**Security layers**](Sources/Security): A reason to make your accounts secure, whether
  local or somewhere online, is to securely encrypt them. OsintgramCXX doesn't take a raw
  configuration file right at the start. You explicitly use the `loginctl` command itself
  to log in.

All modules are being tested on an up-to-date Arch Linux system via `vcpkg`, with eventually
testing the building on a Debian-based system later on via a Dockerfile. The compilers that
are being used are as follows:

- Linux on x86_64 / AMD64: `x86_64-linux-gnu-g++`
- Linux on aarch64: `aarch64-linux-gnu-g++`
- Aarch64 Android builds: `aarch64-linux-android24-clang++`
- x86_64 Android builds: `x86_64-linux-android24-clang++`
- Windows builds: `x86_64-w64-mingw32-g++`

This information can be helpful, when targeting the modules and not knowing, why you are
unable to link them. If compiling on a Windows system is an issue and a Linux system is
required to build them, without having to tackle the Arch Linux installation, use CachyOS as
a safer alternative. Let it even be the installation on a VM with just 4 GB of RAM, if you
have enough to spare.

---

## Modding Support
This feature was originally promised with the Java version back then, where modding support
would be a part of it. The C++ version takes things up a notch and handles modding support
even further. Mods / Plugins can be linked via [`commands.json`](Resources/commands.json)
themselves, in which OsintgramCXX will automatically load modules for you. Take note that
the standard feature set is also a part of the modules that OsintgramCXX relies upon. I will
eventually document the modding support a bit further, when this tool gets finished, as during
the initial developments, things may still change. For now, this would be more than enough.

When writing a module of your own, please note that it is not possible to validate the
parameters of a C symbol of the method that eventually gets invoked. Keep in mind that the
exact parameters need to be used, as demonstrated below, as OsintgramCXX may crash otherwise.

Following this sample, you can create a symbol for a specific command using this example:

```c++
// if using C, skip the <extern "C"> part
extern "C" int cmd_symbol(const char* cmd, int argc, char** argv, int envc, char** env_map) {}
```

As `int main(int argc, char** argv)` is a known thing, this symbol resembles one. However,
the first parameter is the command that you assigned the library for. For `argc` and `argv`,
those are the arguments that end up being passed from the command line. `envc` is the count
of how many in-app environment variables are present, while `env_map` being the in-app
environment. Do not get confused by the term of "environment", as they are not correlated to
Bash environments. Think of one in PowerShell, where you assign a variable via
`$something = some_data_here`.

Alongside, you get the ability to make handlers, one example being that a certain handler
can run immediately after OsintgramCXX finishes loading everything into memory. Certain ones
include the ability of having a handler of immediate execution after loading, those being
`plugin_start`, as demonstrated below. In cases of a `SIGKILL` signal, which cannot be
captured, OsintgramCXX plainly cannot capture and initiate a stop handler, unless given the
capability by the kernel via a flag by the name of `SIGNAL_UNKILLABLE` (don't ask me where I
know that information).

```c++
extern "C" void plugin_start() {}

extern "C" void plugin_stop() {}
```

The `plugin_start` is also effective, if you are preloading commands yourself via C++ code
instead of putting symbols dynamically via `cmd_symbol`. For dynamic preloading, you link
against `app-shell` itself and call on `AddCommand`, which takes in these specific parameters
via the namespace, as demonstrated below via sample code (C++20 syntax):

```c++
#include <AppShell/Shell.hpp>

int my_cmd(const CommandLineInputArgs& args, const ShellEnvironment& env) {
    // here would be your own logic
    // the return code of "int" is designed as a return code
    // for successful execution, return 0.
    return 0;
}

extern "C" void plugin_start() {
    AppShell& instance = AppShell::GetShellInstance();
    instance.AddCommand({
        .name = "my-cmd",
        .description = "This is my own command",
        .handle = my_cmd
    });
}
```

Please do note that when using `AddCommand` directly, your command will not be sorted, as
it usually would, if being used via the C-symbol route.

If you want to snoop around command execution pipelines, you can perform so by making these
additional handlers:

```c++
extern "C" void cmd_handle_start(const char* cmdLine) {}

extern "C" void cmd_handle_finish(const char* cmdLine, int rc, const char* stream) {}
```

With the handle being executed, you are given the values of the raw entered command line
(unparsed), along with being given the return code (`rc`) and the output & error stream
(`stream` from the captured `stdout` & `stderr`).

The possibilities are endless. You can start creating one now, although it is recommended
that you wait, until the version 1 of `commands.json` gets finalized, as it is not finished
and the v1 changes of OsintgramCXX may include changes before release.

Once you have built your own module (by either using `cmake` or manually built with a
compiler), you will need to export it to the commands JSON file. With OsintgramCXX already
built, you're targeting the `Build/bundle/Resources/commands.json` file, unless you are
rebuilding it entirely, in which you can use `Resources/commands.json`. Additionally, you
will need to put your module next to the OsintgramCXX's binary for it to pick your module up.
One sample registration for that would happen to be with this particular structure:

```json
{
    "version": 1,
    "command_sets": [
        {
            "id": 6025,
            "label": "My own module",
            "description": "Description of your own module",
            "author": "Your name here",
            "lib": {
                "linux:x64": "libmymodule.so",
                "linux:arm64": "libmymodule.so",
                "android:x64": "libmymodule.so",
                "android:arm64": "libmymodule.so",
                "windows:x64": "mymodule.dll"
            },
            "handlers": {
                "OnLoad": "plugin_start",
                "OnStop": "plugin_stop",
                "OnCommandExecStart": "cmd_handle_start",
                "OnCommandExecFinish": "cmd_handle_finish"
            },
            "cmd_list": [
                {
                    "cmd": "my-cmd",
                    "description": "Description of my own command",
                    "exec_symbol": "cmd_symbol"
                }
            ]
        }
    ]
}
```

The ID is there to distinguish multiple module identifiers. If an ID has already been loaded,
your module may not load due to clashing module identifiers. Depending on what system
architecture and what system you are running on, along with not planning to cross-compile,
you can leave out other platforms alone. That is, if your system is a Linux x86_64, you will
only need the `linux:x64` under `lib`. The other lib entries are there to separate your own
current system with other system architectures. Please do note that OsintgramCXX is built
with Glibc on top, so if you are using Musl, the C library will most likely be a clashing 
point.

A bit of undocumented, but under `commands.json` under the `command_sets` section, you can
add strings pointing to additional JSON files, effectively avoiding the need of having to
bloat `commands.json` up. Still needs the same format as demonstrated above.

Also keep in mind that I will not support any modifications / plugins that violates the
Instagram ToS harder than this tool already does. If you are unaware of what I mean, refer to
the disclaimer section or let your desired choice of an LLM model read it for you.
Additionally, I am not willing to make an open market for plugins / mods that are considered
"official", "safe to use" or some kind of mod repository in any shape or form.