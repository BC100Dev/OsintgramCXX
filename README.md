# OsintgramCXX
```text
   *@@@%%%@@&,     ,&@@%%%&@@@.  |    (@@&%%%@@@        @&.          ,&@.     
  #@&      ,@@(   #@%.           |  .&@(      \@\      .@@,          *@@,     
 .&@.       /@%  .@@/   .*****,  |  ,@@            @@@@@@@@@@@% .%@@@@@@@@@@%.
  #@&      ,@@/   %@#       @@|  |   &@/      /@/      .@@,          *@@,     
   ,@@@@&@@@&.     *@@@@@@@@@@.  |   \#@@@@&@@@/       .@@,          *@@,     
```

Ever since the development on [the original Project](https://github.com/Datalux/Osintgram)
has stopped and the Instagram API being changed constantly, I have decided to take care
of this project and rewrite most to all components by myself. Well... except for the
Networking part, me and Networking aren't the best friends... just yet. Wait, until I
finish one of the most insane projects known to man. A hint, it's on my GitHub profile.

---

## Use cases
No use cases just yet. Matter of fact, this project is still in active development, and I
am not planning to abandon this project, just like [Datalux](https://github.com/Datalux)
did. However, I will be doing Announcements and Polls, in case I would be disclosing more
information.

Alongside with that, since people were asking me about how to use this tool (you may see
an [example here](https://github.com/BC100Dev/osintgram4j/issues/16) from the previous
version). No offense to people waiting eagerly for the tool to come out, but since actual
development (without AI) does take some time, this won't be a "quick coding session". This
does take time, but I will be updating you guys on any updates.

---

## Disclaimer & Privacy Policy
OsintgramCXX, along with any other software, is merely intended for ethical and legal use
only. By the definition of OSINT, the use of this tool is to collect publicly available
information. This means that commands from the original version (like fetching E-Mail
addresses and phone numbers) are not copied over for legal reasons.

Other developers, myself included, do not support unethical activities like black-hat
hacking (the unlawful version of hacking) and other malicious actions. Always follow
relevant laws and the Terms of Service of the perspective platform.

By using this tool, you acknowledge that:
- **Responsibility**: You are accountable for the actions you take
- **Ethical Use**: You do not misuse this tool to your advantage in harassing people
- **Actions getting logged**: Instagram will log each action you take using this tool
- **Liability**: we, the developers, are not liable for the consequences you take by
  potentially misuing this tool.

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
$ sudo apt install build-essential cmake tar unzip zip curl libssl-dev libcurl4-openssl-dev
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
$ cmake --build Build
```

---

### Building on non-Linux systems
Okay, but you might be running Windows or macOS, so how else can you build it? Different
methods include:

- Virtual Machines
- Cloud Shell (via SSH)
- Containers (e.g. Docker)
- Dual-Booting (if you have a Windows PC)
- WSL (Windows only)
- Termux (Android only, still experimenting for a proper build)