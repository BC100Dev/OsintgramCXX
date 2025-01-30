# OsintgramCXX
```text
   *@@@%%%@@&,     ,&@@%%%&@@@.  |    (@@&%%%@@@        @&.          ,&@.     
  #@&      ,@@(   #@%.           |  .&@(      \@\      .@@,          *@@,     
 .&@.       /@%  .@@/   .*****,  |  ,@@            @@@@@@@@@@@% .%@@@@@@@@@@%.
  #@&      ,@@/   %@#       @@|  |   &@/      /@/      .@@,          *@@,     
   ,@@@@&@@@&.     *@@@@@@@@@@.  |   \#@@@@&@@@/       .@@,          *@@,     
```

Remember, how the beloved Osintgram project by [Datalux](https://github.com/Datalux)
fell apart due to Instagram's API changes and lack of updates? Well, you can stop
worrying. OsintgramCXX aims to fix the issues, and make [the original
Project](https://github.com/Datalux/Osintgram) look like it was never broken. It's
a complete rewrite in C++, bringing back stability, along with new features.

---

## Use cases...?
No use cases yet. Matter of fact, this project is still heavily under development.
Keep an eye on the discussions tab, as I will be doing Announcements, when a new
milestone will be reached.

Oh, and when a project is under development, and features like not being able to log
into your Instagram account is unclear, then do note that features like these are not
fully adjusted yet that it is being used in general use case. And yes, people do
start to piss me off on how to use it. I am tired on telling that this tool is not
ready for public use yet.

Also, since this README is straight up copy-pasted from Osintgram4j, please do note
that I didn't want to rewrite literally everything, since most of these contents here
are just straight up the same thing, but adjusted for the CXX (C++) version.

---

## Disclaimer & Privacy Policy
OsintgramCXX, like any other software, is intended for ethical and legal use only.
Other developers, myself included, do not support unethical activities like hacking
or other malicious actions. Always follow relevant laws and the platforms Terms of
Service.

By using OsintgramCXX, you acknowledge:
- **Responsibility**: You are accountable for your own actions.
- **Ethical Use**: Never misuse this tool or violate Instagram's policies.
- **No Liability**: We, the developers, are not liable for consequences arising from
  misuse.

Don't forget, use OsintgramCXX responsibly, respect privacy of others, and always
act ethically.

---

## Build Process
To build the tool, you will need to install a few tools. Those tools include:
- CMake
- C++ compilers
- VCPKG dependencies (tar, unzip, zip, curl)

For this, using your package manager, install these following packages. This may vary on
your distribution. For Debian (Termux included), you will be using `apt`. Your full
command will be:

```shell
$ sudo apt install build-essential cmake tar unzip zip curl
```

For Arch Linux users:
```shell
$ pacman -S base-devel cmake tar unzip zip curl
```

After installing these dependencies, run these two commands (simplified for VCPKG handling):
```shell
$ git clone https://github.com/BC100Dev/OsintgramCXX.git
$ cd OsintgramCXX
$ chmod +x prepare.sh
$ ./prepare.sh
```

This will download the sources of OsintgramCXX, along with preparing the environment for
building. After the execution of `./prepare.sh`, do the final blow with this command:

```shell
$ cmake --build out
```

This will build the tool itself, and have everything prepared for the execution. Now,
after the build is finished, you can go into the `out` directory and run `./OsintgramCXX`
in itself. However, if you wish to install it directly, you can do so by running:

```shell
$ cmake --install out --prefix /custom/install/path
```

This takes in the `--install out` directory, in which the build has been finished, and
places them in `/usr/local` (Linux / macOS) or `C:\Program Files` on Windows by default.
Modifying the installation path is possible by specifying `--prefix /custom/install/path`
flags.

---

### Building on non-Linux systems
Okay, but you might be running Windows or macOS, so how else can you build it? Different
methods include:

- Virtual Machines
- Cloud ShellFuckery (via SSH)
- Containers (e.g. Docker)
- Dual-Booting (if you have a Windows PC)
- WSL (Windows only)
- Termux (Android only, still experimenting for a proper build)