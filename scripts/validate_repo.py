#!/usr/bin/env python3
"""Repository-level static checks; not a ROS build or hardware test."""

from __future__ import annotations

import re
import sys
from pathlib import Path
from urllib.parse import unquote
from xml.etree import ElementTree

try:
    import yaml
except ImportError:
    print("ERROR: PyYAML is required. Run: python -m pip install -r requirements-dev.txt")
    sys.exit(2)


ROOT = Path(__file__).resolve().parents[1]
ROS_SRC = ROOT / "ros_ws" / "src"
ERRORS: list[str] = []


def error(message: str) -> None:
    ERRORS.append(message)


def relative(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def parse_xml(path: Path) -> ElementTree.ElementTree | None:
    try:
        return ElementTree.parse(path)
    except Exception as exc:  # noqa: BLE001 - report all parser failures together
        error(f"XML parse failed: {relative(path)}: {exc}")
        return None


launch_files = sorted(ROS_SRC.rglob("*.launch"))
package_files = sorted(ROS_SRC.rglob("package.xml"))
xml_trees: dict[Path, ElementTree.ElementTree] = {}
for xml_path in launch_files + package_files:
    tree = parse_xml(xml_path)
    if tree is not None:
        xml_trees[xml_path] = tree


local_packages: dict[str, Path] = {}
for manifest in package_files:
    tree = xml_trees.get(manifest)
    if tree is None:
        continue
    package_root = tree.getroot()
    name = (package_root.findtext("name") or "").strip()
    if not name:
        error(f"Missing package name: {relative(manifest)}")
        continue
    if name in local_packages:
        error(f"Duplicate ROS package name: {name}")
    local_packages[name] = manifest.parent

    maintainer = package_root.find("maintainer")
    maintainer_email = maintainer.get("email", "") if maintainer is not None else ""
    maintainer_name = (maintainer.text or "").strip() if maintainer is not None else ""
    if not maintainer_name or not maintainer_email or ".invalid" in maintainer_email:
        error(f"Invalid maintainer metadata: {relative(manifest)}")

    license_name = (package_root.findtext("license") or "").strip()
    if not license_name or "pending" in license_name.lower():
        error(f"Invalid license metadata: {relative(manifest)}")

    repository_urls = [
        (node.text or "").strip()
        for node in package_root.findall("url")
        if node.get("type") == "repository"
    ]
    if "https://github.com/sws341/ros-mobile-robot-navigation" not in repository_urls:
        error(f"Missing repository URL: {relative(manifest)}")


find_reference = re.compile(r"\$\(find\s+([^)\s]+)\)([^\s\"']*)")
for launch_path in launch_files:
    tree = xml_trees.get(launch_path)
    if tree is None:
        continue
    for element in tree.iter():
        for attribute, value in element.attrib.items():
            for match in find_reference.finditer(value):
                package_name, tail = match.groups()
                if package_name not in local_packages:
                    continue
                if tail and not tail.startswith("/"):
                    error(
                        f"Local $(find ...) path misses '/': {relative(launch_path)} "
                        f"attribute {attribute}={value!r}"
                    )
                    continue
                static_tail = tail.split("$(", 1)[0]
                target = local_packages[package_name] / static_tail.lstrip("/")
                # A dynamic $(arg ...) suffix or an extension-less map-save output
                # names a runtime/output path. Validate its static parent only.
                if "$(" in tail:
                    path_to_check = target if static_tail.endswith("/") else target.parent
                else:
                    path_to_check = target.parent if not Path(tail).suffix else target
                if static_tail and not path_to_check.exists():
                    error(
                        f"Broken local package path: {relative(launch_path)} -> "
                        f"{package_name}{tail}"
                    )


yaml_files = sorted(ROS_SRC.rglob("*.yaml"))
map_references = 0
for yaml_path in yaml_files:
    try:
        data = yaml.safe_load(read_text(yaml_path))
    except Exception as exc:  # noqa: BLE001
        error(f"YAML parse failed: {relative(yaml_path)}: {exc}")
        continue
    if yaml_path.parent.name == "map" and isinstance(data, dict):
        image_value = data.get("image")
        if not image_value or Path(str(image_value)).is_absolute():
            error(f"Map image must be a relative path: {relative(yaml_path)}: {image_value}")
        elif not (yaml_path.parent / str(image_value)).is_file():
            error(f"Map image is missing: {relative(yaml_path)}: {image_value}")
        else:
            map_references += 1


readme = read_text(ROOT / "README.md")
markdown_targets = re.findall(r"!?\[[^\]]*\]\(([^)]+)\)", readme)
local_links = 0
for raw_target in markdown_targets:
    target = raw_target.strip().strip("<>").split()[0]
    if target.startswith(("http://", "https://", "mailto:", "#")):
        continue
    clean_target = unquote(target.split("#", 1)[0])
    if not clean_target:
        continue
    local_links += 1
    if not (ROOT / clean_target).exists():
        error(f"Broken README link: {target}")


expected_figures = {
    "amcl_real_test.png",
    "field_slam_map.png",
    "field_test_scene.jpg",
    "four_wheel_assembled.png",
    "four_wheel_test_platform.jpg",
    "global_path.png",
    "hardware_overview.png",
    "motor_encoder_wiring.png",
    "pid_response.png",
    "robot_prototype.jpg",
    "software_architecture.png",
    "uno_shield_wiring.png",
}
figure_dir = ROOT / "assets" / "figures"
actual_figures = {path.name for path in figure_dir.iterdir() if path.is_file()}
if actual_figures != expected_figures:
    error(
        "Figure inventory mismatch: "
        f"missing={sorted(expected_figures - actual_figures)}, "
        f"unexpected={sorted(actual_figures - expected_figures)}"
    )
for figure_name in sorted(expected_figures):
    figure = figure_dir / figure_name
    if figure_name not in readme:
        error(f"Figure is not referenced by README: {figure_name}")
        continue
    header = figure.read_bytes()[:12]
    valid_png = figure.suffix.lower() == ".png" and header.startswith(b"\x89PNG\r\n\x1a\n")
    valid_jpeg = figure.suffix.lower() in {".jpg", ".jpeg"} and header.startswith(b"\xff\xd8\xff")
    if not (valid_png or valid_jpeg):
        error(f"Invalid image signature: {relative(figure)}")


blocked_suffixes = {".zip", ".rar", ".7z", ".doc", ".docx", ".ppt", ".pptx"}
ignored_parts = {".git", ".venv", "__pycache__"}
text_suffixes = {
    "",
    ".cff",
    ".h",
    ".ino",
    ".launch",
    ".md",
    ".ps1",
    ".py",
    ".txt",
    ".xml",
    ".xacro",
    ".yaml",
    ".yml",
}
absolute_user_path = re.compile(r"(?i)(?:[a-z]:\\Users\\[^\\\s]+|/home/[^/\s]+/)")
secret_assignment = re.compile(
    r"(?i)(?:api[_-]?key|access[_-]?token|password|passwd|secret)\s*[:=]\s*['\"][^'\"]+"
)
placeholders = ("maintainer@example", "Maintainer TODO", "License-selection-pending", "Project author")

for path in ROOT.rglob("*"):
    if not path.is_file() or any(part in ignored_parts for part in path.parts):
        continue
    if path.resolve() == Path(__file__).resolve():
        continue
    if path.suffix.lower() in blocked_suffixes:
        error(f"Private/source archive format must not be published: {relative(path)}")
    if path.suffix.lower() not in text_suffixes:
        continue
    try:
        content = read_text(path)
    except UnicodeDecodeError:
        continue
    if absolute_user_path.search(content):
        error(f"Absolute user path found: {relative(path)}")
    if secret_assignment.search(content):
        error(f"Possible embedded secret found: {relative(path)}")
    for placeholder in placeholders:
        if placeholder in content:
            error(f"Release placeholder {placeholder!r} found: {relative(path)}")


firmware_path = ROOT / "firmware" / "ROSArduinoBridge4wd" / "ROSArduinoBridge4wd.ino"
controller_path = ROOT / "firmware" / "ROSArduinoBridge4wd" / "diff_controller.h"
arduino_params_path = ROOT / "ros_ws" / "src" / "car_start" / "config" / "arduino_params.yaml"
firmware = read_text(firmware_path)
controller = read_text(controller_path)
arduino_params = yaml.safe_load(read_text(arduino_params_path))

encoder_case = firmware.split("case READ_ENCODERS:", 1)[-1].split("break;", 1)[0]
if "readEncoder(LEFT2)" in encoder_case or "readEncoder(RIGHT2)" in encoder_case:
    error("READ_ENCODERS emits rear-wheel values and can break the two-value host protocol")
if isinstance(arduino_params, dict) and "arduino_led" in (arduino_params.get("sensors") or {}):
    error("D13 arduino_led output conflicts with RIGHT_MOTOR_FORWARD")
for gain in ("Kp", "Kd", "Ki", "Ko"):
    if not re.search(rf"\b(?:float|double)\s+{gain}\s*=", controller):
        error(f"PID gain {gain} is not floating point")
if "atof(str)" not in firmware:
    error("UPDATE_PID does not preserve fractional PID values")


required_files = (
    "LICENSE",
    "NOTICE",
    "CITATION.cff",
    "VERSION",
    "docs/LICENSE_SCOPE.md",
    "docs/THIRD_PARTY.md",
    "firmware/THIRD_PARTY_LICENSE_BSD.txt",
)
for required in required_files:
    if not (ROOT / required).is_file():
        error(f"Required release file is missing: {required}")


print(
    "Checked "
    f"{len(launch_files)} launch XML files, {len(package_files)} package manifests, "
    f"{len(yaml_files)} YAML files, {map_references} map references, "
    f"{local_links} README local links, and {len(expected_figures)} figures."
)
if ERRORS:
    print("\n".join(f"ERROR: {message}" for message in ERRORS))
    sys.exit(1)
print("PASS: repository static validation succeeded; build and hardware runtime remain untested.")
