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
To build OsintgramCXX, you need a Linux environment with `vcpkg` installed. If you
haven't installed `vcpkg` yet, then fetch [the `vcpkg` repo](https://github.com/microsoft/vcpkg).
Optionally, you can also put the `vcpkg` command into the PATH environment for easy access,
but it is not necessary.

```bash
$ vcpkg integrate install
```

If you see something like this:

```
Applied user-wide integration for this vcpkg root.
CMake projects should use: "-DCMAKE_TOOLCHAIN_FILE=/home/bc100dev/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake"
```

Then you are good to go. Make sure to copy the CLI argument that it also provides, as
you will need it.

From there, install `libcurl`, as it is used to make Network Requests. For those unfamiliar,
run this:

```bash
$ vcpkg install curl
```

If you plan to cross-compile for aarch64 (arm64), then you can append `:arm64-linux`, resulting
in `curl:arm64-linux`. From there, clone the OsintgramCXX repository:

```bash
$ git clone https://github.com/BeChris100/OsintgramCXX
$ cd OsintgramCXX
```

Now, you are ready to do the final process: actually building it. You can do this by
doing the following:

```bash
$ cmake -S . -B Build -DCMAKE_TOOLCHAIN_FILE="/path/to/vcpkg/you/copied"
$ cmake --build Build
```

Remember that one CLI argument that `vcpkg integrate install` told you? Yeah, replace
that `-DCMAKE_TOOLCHAIN_FILE="/path/to/vcpkg/you/copied` with that argument that `vcpkg`
is telling you to. In my case, it would be
`-DCMAKE_TOOLCHAIN_FILE=/home/bc100dev/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake`,
since I had it downloaded automatically by CLion.

This process will build the `OsintgramCXX` executable under the `Build` directory. That is,
if you have decided to change the `-B Build` to something like `-B out`, then `out` is the
output directory, in which the executable will be living under.

Of course, if you wish to cross-compile to aarch64, you NEED to add another following
argument, being `-DVCPKG_TARGET_TRIPLET=arm64-linux`, along with
`-DAPP_CONFIG_TARGET=Linux_arm64` to point to the compilers. This will tell `vcpkg` and `cmake`
to use the binaries from this triplet. Applying it programmatically in the CMake is not possible,
since the Toolchain file gets sourced first before the project's CMake.

Okay, but you might be running Windows or macOS, so how else can you build it? Different
methods include:

- Virtual Machines
- Cloud Shell (via SSH)
- Containers (e.g. Docker)
- Dual-Booting (if you have a Windows PC)
- WSL (Windows only)
- Termux (Android only, still experimenting for a proper build)