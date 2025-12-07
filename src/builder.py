import json
import os
import shutil
import subprocess
import requests
from pathlib import Path
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("--python-version", default="3.12")
parser.add_argument("--taglib-version", default="1.13.1")
parser.add_argument("--boost-version", default="1.89.0")
parser.add_argument("--with-gdb", action="store_true")
parser.add_argument("--build-wheel", action="store_true")

wheel_version = (
    Path(__file__)
    .parent.parent.joinpath("tagpy", "__version__.txt")
    .open()
    .read()
    .strip()
)

args = parser.parse_args()

root_folder = Path(__file__).absolute().parent.parent
build_folder = root_folder.joinpath("build")
build_folder.mkdir(exist_ok=True)
manifest_path = build_folder.joinpath("versions-manifest.json")

if not manifest_path.exists():
    print("downloading manifest")
    manifest_data = requests.get(
        "https://raw.githubusercontent.com/MarkusJx/prebuilt-boost/refs/heads/main/versions-manifest.json"  # noqa: E501
    )
    manifest_data.raise_for_status()
    with manifest_path.open("w") as f:
        f.write(manifest_data.text)
        manifest = manifest_data.json()
else:
    manifest = json.load(manifest_path.open())

available_versions = set([v["version"] for v in manifest])
if args.boost_version not in available_versions:
    raise Exception(
        f"Can't find {args.boost_version}. Available: {sorted(available_versions)}"
    )
versioned_manifest = [v for v in manifest if v["version"] == args.boost_version][0]
files = sorted(
    [f for f in versioned_manifest["files"] if f["platform"] == "linux"],
    key=lambda f: f["platform_version"],
    reverse=True,
)[0]

boost_path = build_folder.joinpath(files["filename"])

if not boost_path.exists():
    print("downloading", files["download_url"], boost_path)
    boost = requests.get(files["download_url"])
    boost.raise_for_status()
    with boost_path.open("wb") as f:
        f.write(boost.content)

boost_version_folder = build_folder.joinpath(f"boost-{args.boost_version}")
if not boost_version_folder.exists():
    print(f"Unpacking boost {args.boost_version}")
    subprocess.check_call(["tar", "-zxf", boost_path.as_posix()], cwd=build_folder)
    boost_unversioned_folder = build_folder.joinpath("boost")
    shutil.move(boost_unversioned_folder, boost_version_folder)

taglib_path = build_folder.joinpath(f"taglib-{args.taglib_version}.tar.gz")
if not taglib_path.exists():
    taglib_url = f"https://taglib.org/releases/taglib-{args.taglib_version}.tar.gz"
    print("downloading", taglib_url)
    taglib = requests.get(taglib_url)
    taglib.raise_for_status()
    with taglib_path.open("wb") as f:
        f.write(taglib.content)

taglib_version_folder = build_folder.joinpath(f"taglib-{args.taglib_version}")
if not taglib_version_folder.exists():
    print(f"Unpacking taglib {args.taglib_version}")
    subprocess.check_call(["tar", "-zxf", taglib_path.as_posix()], cwd=build_folder)

taglib_build_version_folder = build_folder.joinpath(
    f"taglib-{args.taglib_version}-built"
)

makefile_path = taglib_version_folder.joinpath("Makefile")
if not makefile_path.exists():
    if taglib_version_folder.joinpath("CMakeLists.txt").exists():
        subprocess.check_call(
            [
                "cmake",
                f"-DCMAKE_INSTALL_PREFIX={taglib_build_version_folder}",
                "-DCMAKE_POSITION_INDEPENDENT_CODE=ON",
                ".",
            ],
            cwd=taglib_version_folder,
        )
    elif taglib_version_folder.joinpath("configure").exists():
        subprocess.check_call(
            [
                "./configure",
            ],
            cwd=taglib_version_folder,
        )
    else:
        raise Exception

taglib_library = taglib_version_folder.joinpath("taglib", "libtag.a")
if not taglib_library.exists():
    subprocess.check_call(["make"], cwd=taglib_version_folder)

if not taglib_build_version_folder.exists():
    subprocess.check_call(["make", "install"], cwd=taglib_version_folder)

venv_folder = build_folder.joinpath(f"venv-{args.python_version}")
if not venv_folder.exists():
    subprocess.check_call(["uv", "venv", "--python", args.python_version, venv_folder])

venv_bin_folder = venv_folder.joinpath("bin")
pip_path = venv_bin_folder.joinpath("pip3")
if not pip_path.exists():
    subprocess.check_call(
        [venv_bin_folder.joinpath("python"), "-m", "ensurepip"], cwd=root_folder
    )

my_env = os.environ.copy()
my_env["VIRTUAL_ENV"] = venv_folder.as_posix()
my_env["CPPFLAGS"] = (
    my_env.get("CPPFLAGS", "")
    + f" -coverage -I{boost_version_folder}/include"
    + f" -I{taglib_build_version_folder}/include"
)
my_env["LDFLAGS"] = ""
extra_library_paths_list = []
for folder in [boost_version_folder, taglib_build_version_folder]:
    my_env["LDFLAGS"] += f" -L{folder}/lib -L{folder}/lib64"
    extra_library_paths_list.append(f"{folder}/lib")
    extra_library_paths_list.append(f"{folder}/lib64")
extra_library_paths = ":".join(extra_library_paths_list)
if "LD_LIBRARY_PATH" in my_env:
    my_env["LD_LIBRARY_PATH"] += f":{extra_library_paths}"
else:
    my_env["LD_LIBRARY_PATH"] = extra_library_paths

subprocess.check_call(
    ["uv", "pip", "install", "-r", "requirements-dev.txt"], cwd=root_folder, env=my_env
)
subprocess.check_call([pip_path, "install", "."], cwd=root_folder, env=my_env)
command: list[str] = [
    venv_bin_folder.joinpath("python").as_posix(),
    "-m",
    "pytest",
    "-vvv",
    "--cov=tagpy",
    "--cov-report=term-missing",
    "--cov-report=lcov",
    "--cov-fail-under=50",
]
if args.with_gdb:
    command = ["gdb", "--args"] + command
print(" ".join(command))
subprocess.check_call(
    command,
    cwd=root_folder,
    env=my_env,
)

if args.build_wheel:
    subprocess.check_call(
        [venv_bin_folder.joinpath("python").as_posix(), "-m", "build"],
        cwd=root_folder,
        env=my_env,
    )

    compact_python_version = args.python_version.replace(".", "")

    subprocess.check_call(
        [
            "auditwheel",
            "repair",
            f"dist/tagpy-{wheel_version}-cp{compact_python_version}-cp{compact_python_version}-linux_x86_64.whl",
        ],
        cwd=root_folder,
        env=my_env,
    )
