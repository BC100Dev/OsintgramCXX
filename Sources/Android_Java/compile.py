#!/usr/bin/env python3

import os
import glob
import shutil
import subprocess
import sys


def resolve_sdk() -> str:
    _ANDROID_HOME = os.environ.get("ANDROID_HOME")
    if _ANDROID_HOME:
        return _ANDROID_HOME

    _ANDROID_HOME = os.environ.get("ANDROID_SDK_ROOT")
    if _ANDROID_HOME:
        return _ANDROID_HOME

    ## attempt to resolve path automatically
    _HOME_MAP = {
        "linux": os.environ.get("HOME") + "/Android/Sdk",
        "win32": [
            (os.environ.get("LOCALAPPDATA") or "") + "\\Android\\Sdk",
            (os.environ.get("USERPROFILE") or "") + "\\Android\\Sdk",
        ],
        "darwin": os.environ.get("HOME") + "/Library/Android/Sdk"
    }

    if type(_HOME_MAP[sys.platform]) == str and os.path.exists(_HOME_MAP[sys.platform]):
        return _HOME_MAP[sys.platform]
    else:
        for it in _HOME_MAP["win32"]:
            if os.path.exists(it):
                return it

    return None


ANDROID_HOME = resolve_sdk()
CMAKE_BINARY_DIR = os.environ.get("CMAKE_BINARY_DIR", "build")

JAVA_RELEASE = "11"
MIN_API = "24"

SRC_DIR = os.path.join(os.path.dirname(__file__), "src")
CLASS_DIR = os.path.join(CMAKE_BINARY_DIR, "java_classes")
DEX_DIR = os.path.join(CMAKE_BINARY_DIR, "java_dex")


def resolve_javac() -> str:
    java_home = os.environ.get("JAVA_HOME")
    if java_home:
        candidate = os.path.join(java_home, "bin", "javac")
        if os.path.isfile(candidate) and os.access(candidate, os.X_OK):
            return candidate

    found = shutil.which("javac")
    if found:
        return found

    print("[ERROR] javac not found. Install a JDK or set JAVA_HOME.", file=sys.stderr)
    sys.exit(1)


def resolve_d8() -> str:
    if not ANDROID_HOME:
        print("[ERROR] ANDROID_HOME or ANDROID_SDK_ROOT is not set.", file=sys.stderr)
        sys.exit(1)

    build_tools_root = os.path.join(ANDROID_HOME, "build-tools")
    if not os.path.isdir(build_tools_root):
        print(f"[ERROR] build-tools directory not found at {build_tools_root}.", file=sys.stderr)
        sys.exit(1)

    versions = sorted(os.listdir(build_tools_root), reverse=True)
    for version in versions:
        candidate = os.path.join(build_tools_root, version, "d8")
        if os.path.isfile(candidate) and os.access(candidate, os.X_OK):
            print(f"[INFO] Using d8 from build-tools {version}")
            return candidate

    print("[ERROR] d8 not found in any build-tools version.", file=sys.stderr)
    sys.exit(1)


def resolve_android_jar() -> str:
    if not ANDROID_HOME:
        print("[ERROR] ANDROID_HOME or ANDROID_SDK_ROOT is not set.", file=sys.stderr)
        sys.exit(1)

    platforms_root = os.path.join(ANDROID_HOME, "platforms")
    if not os.path.isdir(platforms_root):
        print(f"[ERROR] platforms directory not found at {platforms_root}.", file=sys.stderr)
        sys.exit(1)

    versions = sorted(os.listdir(platforms_root), reverse=True)
    for version in versions:
        candidate = os.path.join(platforms_root, version, "android.jar")
        if os.path.isfile(candidate):
            print(f"[INFO] Using android.jar from {version}")
            return candidate

    print("[ERROR] android.jar not found in any platform version.", file=sys.stderr)
    sys.exit(1)


javac = resolve_javac()
d8 = resolve_d8()
android_jar = resolve_android_jar()

os.makedirs(CLASS_DIR, exist_ok=True)
os.makedirs(DEX_DIR, exist_ok=True)

sources = glob.glob(os.path.join(SRC_DIR, "**", "*.java"), recursive=True)
if not sources:
    print("[ERROR] No .java files found.", file=sys.stderr)
    sys.exit(1)

print(f"[INFO] Compiling {len(sources)} source file(s)...")
result = subprocess.run([javac, "--release", JAVA_RELEASE, "-d", CLASS_DIR] + sources)

if result.returncode != 0:
    print("[ERROR] javac failed.", file=sys.stderr)
    sys.exit(result.returncode)

classes = glob.glob(os.path.join(CLASS_DIR, "**", "*.class"), recursive=True)
print(f"[INFO] Converting {len(classes)} class file(s) to DEX...")
result = subprocess.run([d8, "--min-api", MIN_API, "--classpath", android_jar, "--output", DEX_DIR] + classes)

if result.returncode != 0:
    print("[ERROR] d8 failed.", file=sys.stderr)
    sys.exit(result.returncode)

os.makedirs(os.path.join(CMAKE_BINARY_DIR, "bundle", "android"), exist_ok=True)
shutil.move(os.path.join(DEX_DIR, "classes.dex"), os.path.join(CMAKE_BINARY_DIR, "bundle", "android", "classes.dex"))
print(f"[OK] classes.dex written to {CMAKE_BINARY_DIR}/bundle/classes.dex")
