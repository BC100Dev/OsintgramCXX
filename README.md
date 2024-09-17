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
Prepare a Linux environment, with either one of these options:
- Host (main OS / Dual Boot)
- VM (Virtual Machine)
- Cloud Shell / Servers (Google Cloud, Linode)
- Containers (Docker)
- WSL (Windows-only)

This project is mainly focused and optimized for **the Linux platform**. Simple
setup with the Shell scripts:

1. ***Download dependencies***:
    - Debian/Ubuntu based: `sudo apt install build-essential cmake tar wget`
    - Arch/Manjaro based: `sudo pacman -Sy base-devel cmake tar wget`
2. ***Clone the repo***: `git clone https://github.com/BeChris100/OsintgramCXX`
3. ***Change the directory***: `cd OsintgramCXX`
4. ***Set up environment***: `mkdir Build && cmake -S $(pwd) -B $(pwd)/Build`
5. ***Build***: `cmake --build $(pwd)/Build --target all -- -j$(nproc)`

The built binaries will be found in the directory of the Build. Change the directory
to the `Build` directory (or however you named it), and you can run `./OsintgramCXX`
from there.