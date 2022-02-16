from distutils.sysconfig import get_python_lib

import sipbuild
from sipbuild.module import abi_version

print("sip_version:%06.0x" % sipbuild.version.SIP_VERSION)
print("sip_version_num:%d" % sipbuild.version.SIP_VERSION)
print("sip_version_str:%s" % sipbuild.version.SIP_VERSION_STR)
print("sip_abi_version:%s" % abi_version.resolve_abi_version(None) + ".0")

python_modules_dir = get_python_lib(plat_specific=1)
print("default_sip_dir:%s" % python_modules_dir)
