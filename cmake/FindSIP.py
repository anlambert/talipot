import sipbuild
from sipbuild.module.abi_version import (
    resolve_abi_version,
    get_sip_module_version
)

print("sip_version:%06.0x" % sipbuild.version.SIP_VERSION)
print("sip_version_num:%d" % sipbuild.version.SIP_VERSION)
print("sip_version_str:%s" % sipbuild.version.SIP_VERSION_STR)
abi_major_version = resolve_abi_version(abi_version=None).split('.')[0]
print("sip_module_version:%s" % get_sip_module_version(abi_major_version))
