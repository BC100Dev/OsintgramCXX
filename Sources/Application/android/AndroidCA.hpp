#ifndef OSINTGRAMCXX_ANDROIDCA_HPP
#define OSINTGRAMCXX_ANDROIDCA_HPP

#include <string>

namespace OsintgramCXX::AndroidVer {

    // Prepare the CA certificates into one file, so that libcurl can understand.
    // This is a required step with Android devices because they don't use the same structure as the desktop versions.
    // This method can also be called on Linux (non-Android) and Windows, but will be rendered ineffective, as if "doNothing()" has been called.
    // If libcurl doesn't find the PEM file that libcurl can rely on, this will most likely fail.
    // If called on Android devices, a new file under "{ExecutablePath}/android_certstore.pem" will spawn, which merges all files from "/system/etc/security/cacerts".
    void prepare_cacerts();

}

#endif //OSINTGRAMCXX_ANDROIDCA_HPP
