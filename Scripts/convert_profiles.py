import json

INPUT_FILE = "../References/device_profiles_ref.json"
OUTPUT_FILE = "../Resources/device_profiles.json"

with open(INPUT_FILE, "r", encoding="utf-8") as f:
    raw_lines = json.load(f)

parsed_devices = []

for entry in raw_lines:
    parts = [p.strip() for p in entry.split(";")]
    if len(parts) != 7:
        continue

    try:
        api_ver, os_ver = parts[0].split("/")
        width, height = map(int, parts[2].split("x"))
        parsed_devices.append({
            "android_api": int(api_ver),
            "android_version": os_ver,
            "display_dpi": parts[1],
            "display_size": {
                "width": width,
                "height": height
            },
            "manufacturer": parts[3],
            "product": parts[4],
            "codename": parts[5],
            "cpu_label": parts[6]
        })
    except Exception as e:
        print(f"[WARN] Could not parse {entry}: {e}")

with open(OUTPUT_FILE, "w", encoding="utf-8") as out:
    json.dump(parsed_devices, out, indent=4)

print(f"[OK] Wrote {len(parsed_devices)} entries to {OUTPUT_FILE}")
