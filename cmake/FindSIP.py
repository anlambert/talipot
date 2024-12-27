import sipbuild
from sipbuild.module.abi_version import get_sip_module_version, resolve_abi_version

print("sip_version:%06.0x" % sipbuild.version.SIP_VERSION)
print("sip_version_num:%d" % sipbuild.version.SIP_VERSION)
print("sip_version_str:%s" % sipbuild.version.SIP_VERSION_STR)
abi_version = resolve_abi_version(abi_version=None)
abi_major_version = abi_version.split(".")[0]
print("sip_abi_version:%s" % abi_version)
print("sip_module_version:%s" % get_sip_module_version(abi_major_version))
