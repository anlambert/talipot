import os

import sipbuild

sip_version = tuple(map(int, sipbuild.version.SIP_VERSION_STR.split(".")))

if sip_version >= (6, 10):
    from sipbuild.module.abi_version import get_latest_version, get_module_source_dir

    abi_major_version = get_latest_version()
    abi_minor_version = get_latest_version(abi_major_version)
    abi_version = f"{abi_major_version}.{abi_minor_version}"
    module_source_dir = get_module_source_dir((abi_major_version, abi_minor_version))
    with open(os.path.join(module_source_dir, "sip.h.in")) as vf:
        for line in vf:
            parts = line.strip().split()
            if len(parts) == 3 and parts[0] == "#define":
                _, name, value = parts
                if name == "SIP_MODULE_PATCH_VERSION":
                    abi_patch_version = value
                    break
    sip_module_version = f"{abi_major_version}.{abi_minor_version}.{abi_patch_version}"
else:
    from sipbuild.module.abi_version import get_sip_module_version, resolve_abi_version

    abi_version = resolve_abi_version(abi_version=None)
    abi_major_version = abi_version.split(".")[0]
    sip_module_version = get_sip_module_version(abi_major_version)

print(f"sip_version:{sipbuild.version.SIP_VERSION:06x}")
print(f"sip_version_num:{sipbuild.version.SIP_VERSION}")
print(f"sip_version_str:{sipbuild.version.SIP_VERSION_STR}")

print(f"sip_abi_version:{abi_version}")
print(f"sip_module_version:{sip_module_version}")
