import os
from pathlib import Path
import re
import subprocess
from collections import defaultdict
import sys

base_dir = sys.argv[1]
frame_pattern = re.compile(r"(.+)_frame\d+\.(png|jpg|jpeg)$")

frames = defaultdict(list)
individual_files = []

for root, _, filenames in os.walk(base_dir):
    for name in sorted(filenames):
        path = os.path.join(root, name).replace("\\", "/")
        match = frame_pattern.match(name)
        if match:
            key = match.group(1)
            key_path = Path(os.path.join(root, key).replace("\\", "/")).stem
            frames[key_path].append(path)
        else:
            individual_files.append(path)

params = ""
for file in individual_files:
    print(file + "\n")
    params += f'"{file}" '
    
for key, paths in frames.items():
    # Sort the paths to ensure consistent ordering
    paths.sort()
    
    # Add the paths to the params string
    params += f'"{key}"="'
    for path in paths:
        print(path + "\n")
        params += f'{path} '

    params += '" '

subprocess.run(f"python -m textureatlas -o {sys.argv[2]}.png -m {sys.argv[2]}.map -mf binary " + params)
