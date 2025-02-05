#!/bin/bash

find ./demos/ -iname *.h -o -iname *.cpp -o -iname *.cxx | xargs clang-format -i
find ./library/ -iname *.h -o -iname *.cpp -o -iname *.cxx | xargs clang-format -i
find ./plugins/ -iname *.h -o -iname *.cpp -o -iname *.cxx | xargs clang-format -i
find ./software/ -iname *.h -o -iname *.cpp -o -iname *.cxx | xargs clang-format -i
find ./tests/ -iname *.h -o -iname *.cpp -o -iname *.cxx | xargs clang-format -i
find ./library/ -iname *.sip -o -iname *.sip.in | xargs python3 ./utils/scripts/format_sip_file.py
