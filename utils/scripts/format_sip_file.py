# Copyright (C) 2023  The Talipot developers
#
# Talipot is a fork of Tulip, created by David Auber
# and the Tulip development Team from LaBRI, University of Bordeaux
#
# See the AUTHORS file at the top-level directory of this distribution
# License: GNU General Public License version 3, or any later version
# See top-level LICENSE file for more information

# Format sip files using rstfmt for the docstrings and clang-format
# for c++ source code

import os
import shutil
import sys
import tempfile
from subprocess import run

new_sip_file_path = tempfile.mktemp()
temp_file_path = None
temp_file = None

for sip_file_path in sys.argv[1:]:
    with open(sip_file_path, "r") as sip_file, open(
        new_sip_file_path, "w"
    ) as new_sip_file:
        extracting_rst = False
        extracting_cpp = False
        for line in sip_file:
            extracting_code = extracting_rst or extracting_cpp
            if line.startswith("%Docstring"):
                new_sip_file.write(line)
                extracting_rst = True
                temp_file_path = tempfile.mktemp()
                temp_file = open(temp_file_path, "w")
                continue
            elif line.startswith(
                (
                    "%ModuleHeaderCode",
                    "%ModuleCode",
                    "%MethodCode",
                    "%ConvertToTypeCode",
                    "%ConvertFromTypeCode",
                    "%ConvertToSubClassCode",
                    "%GetCode",
                )
            ):
                new_sip_file.write(line)
                extracting_cpp = True
                temp_file_path = tempfile.mktemp(dir=os.path.dirname(sip_file_path))
                temp_file = open(temp_file_path, "w")
                continue
            elif line.startswith("%End") and extracting_code:
                temp_file.close()
                if extracting_rst:
                    run(["rstfmt", "-w", "100", temp_file_path])
                elif extracting_cpp:
                    run(["clang-format", "-i", temp_file_path])
                with open(temp_file_path, "r") as f:
                    new_sip_file.write(f.read())
                    new_sip_file.write(line)
                os.remove(temp_file_path)
                extracting_cpp = False
                extracting_rst = False
            elif extracting_code:
                temp_file.write(line)
            else:
                new_sip_file.write(line)
    shutil.copyfile(new_sip_file_path, sip_file_path)
