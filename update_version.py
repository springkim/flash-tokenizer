import os
import re

with open("pyproject.toml", "rt", encoding="utf-8") as f:
    lines = f.read().splitlines()

for line in lines:
    if line.startswith("version"):
        version_toml = line.split("=")[1].strip()[1:-1]
        break

with open("CMakeLists.txt", "rt", encoding="utf-8") as f:
    lines = f.read().splitlines()
for line in lines:
    if "PROJECT_VERSION_STR" in line:
        tmp = re.findall('([^)]+)', line)[0]
        version_cmakelist = re.sub(r'[^0-9\.]', '', tmp)
        break

print(f'pyptoject.toml: {version_toml}')
print(f'CMakeLists.txt: {version_cmakelist}')
if version_toml != version_cmakelist:
    print("Incorrect Version")

new_version = input("Enter new version: ")

print(f'Version: {version_toml} => {new_version}')
check = input('Update version(y,n): ')
if check == "y":
    for file in ['pyproject.toml', 'CMakeLists.txt', 'include/version.h']:
        with open(file, "rt", encoding="utf-8") as f:
            data = f.read().replace(version_toml, new_version)
        with open(file, "wt", encoding="utf-8") as f:
            f.write(data)
else:
    print("Cancel update")
