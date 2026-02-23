#!/usr/bin/env python3
"""
Strict Doxygen Linter for Modern C++ Projects

- Scans directory recursively
- Applies Doxygen compliance checks
- Writes failures to FAILED_DOXYGEN.md
- Returns non-zero exit code if violations found
"""

from __future__ import annotations

import os
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import List, Tuple, Iterable

# ============================================================
# ===================== CONFIGURATION ========================
# ============================================================

PROJECT_ROOT: Path = Path.cwd()

# Directories to ignore
EXCLUDED_DIRS: Tuple[str, ...] = (
    "out",
    ".vscode",
    "cmake",
    "example",
    "scripts",
    ".git",
    "test",
    "third_party"
)

# File extensions to scan
SCAN_EXTENSIONS: Tuple[str, ...] = (
    ".h",
    ".hpp",
)

# File extensions to exclude explicitly
EXCLUDED_EXTENSIONS: Tuple[str, ...] = ()

MAX_LINE_LENGTH: int = 100

FAILED_OUTPUT_FILE: Path = PROJECT_ROOT / "FAILED_DOXYGEN.md"

# ============================================================
# ====================== REGEX RULES =========================
# ============================================================

FIRST_PERSON_PATTERN = re.compile(r"\b(we|our|my)\b|\bI\b(?![/oO])", re.IGNORECASE)
FUTURE_TENSE_PATTERN = re.compile(r"\bwill\b", re.IGNORECASE)

FUNCTION_PATTERN = re.compile(
    r'^\s*(?:template\s*<[^>]*>\s*)?(?:virtual\s+)?(?:inline\s+)?(?:explicit\s+)?(?:static\s+)?(?:constexpr\s+)?([\w:<>\*&\s]+?)(?:\s+const)?\s+(?!if|for|while|switch|return|static_assert|catch)(\w+)\s*\(([^)]*)\)(?:\s*const\s*)?(?:\s*override\s*)?(?:\s*final\s*)?(?:\s*=\s*0)?\s*[;{]',
    re.MULTILINE,
)

# Keywords that are not functions
NON_FUNCTION_KEYWORDS = {
    'if', 'for', 'while', 'switch', 'return', 'static_assert',
    'catch', 'sizeof', 'decltype', 'typeid', 'alignof', 'alignas',
    'typename', 'namespace', 'using', 'typedef', 'struct', 'class',
    'enum', 'union', 'template', 'extern', 'thread_local', 'mutable',
    'volatile', 'const', 'constexpr', 'consteval', 'constinit',
    'concept', 'requires', 'co_await', 'co_yield', 'co_return',
    'public', 'private', 'protected', 'friend', 'virtual', 'inline',
    'explicit', 'static', 'register', 'auto', 'signed', 'unsigned',
    'true', 'false', 'nullptr', 'and', 'or', 'not', 'xor', 'bitand',
    'bitor', 'compl', 'not_eq', 'and_eq', 'or_eq', 'xor_eq',
}

DOXYGEN_BLOCK_PATTERN = re.compile(r"/\*\*.*?\*/", re.DOTALL)
ENUM_STRUCT_PATTERN = re.compile(r'\b(enum|struct|class)\s+(\w+)')

# Core required tags - must be present
CORE_REQUIRED_TAGS = (
    "@brief",
)

# Conditional tags - only check if they make sense for the function
OPTIONAL_TAGS = (
    "@throws",
    "@note",
    "@warning",
    "@since",
    "@ingroup",
)

# ============================================================
# ======================= DATA TYPES =========================
# ============================================================


@dataclass
class Violation:
    file: Path
    message: str
    symbol: str | None = None

    def to_markdown(self) -> str:
        if self.symbol:
            return f"- **{self.file}** :: `{self.symbol}` → {self.message}"
        return f"- **{self.file}** → {self.message}"


# ============================================================
# ======================= CORE LOGIC =========================
# ============================================================


def should_skip(path: Path) -> bool:
    """Determine if a file should be skipped."""
    if path.suffix not in SCAN_EXTENSIONS:
        return True
    if path.suffix in EXCLUDED_EXTENSIONS:
        return True
    for part in path.parts:
        if part in EXCLUDED_DIRS:
            return True
    return False


def read_file(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="ignore")


def check_line_length(content: str, file: Path) -> List[Violation]:
    violations: List[Violation] = []
    for i, line in enumerate(content.splitlines(), start=1):
        if len(line) > MAX_LINE_LENGTH:
            violations.append(
                Violation(file, f"Line {i} exceeds {MAX_LINE_LENGTH} characters")
            )
    return violations


def check_file_header(content: str, file: Path) -> List[Violation]:
    if not re.search(r"/\*\*.*?@file.*?\*/", content, re.DOTALL):
        return [Violation(file, "Missing file-level @file header")]
    return []


def is_private_type(content: str, type_pos: int) -> bool:
    """Check if an enum/struct/class is in a private section."""
    before_type = content[:type_pos]

    # Search backwards for access specifiers
    private_matches = list(re.finditer(r'\bprivate\s*:', before_type))
    protected_matches = list(re.finditer(r'\bprotected\s*:', before_type))
    public_matches = list(re.finditer(r'\bpublic\s*:', before_type))

    if not private_matches and not protected_matches and not public_matches:
        return False

    # Get the most recent access specifier
    last_private = private_matches[-1].end() if private_matches else -1
    last_protected = protected_matches[-1].end() if protected_matches else -1
    last_public = public_matches[-1].end() if public_matches else -1

    last_access = max(last_private, last_protected, last_public)

    return last_access == last_private


def check_enum_struct_docs(content: str, file: Path) -> List[Violation]:
    violations: List[Violation] = []
    for match in ENUM_STRUCT_PATTERN.finditer(content):
        name = match.group(2)
        type_pos = match.start()

        # Skip private types (like private unions inside classes)
        if is_private_type(content, type_pos):
            continue

        # Increase search range to 1000 characters
        start = max(0, type_pos - 1000)
        snippet = content[start:type_pos]
        if "/**" not in snippet and "///" not in snippet:
            violations.append(Violation(file, "Missing Doxygen comment", name))
    return violations


def is_private_function(content: str, func_pos: int) -> bool:
    """Check if a function is in a private section."""
    # Find the last access specifier before this function
    before_func = content[:func_pos]

    # Search backwards for access specifiers
    private_matches = list(re.finditer(r'\bprivate\s*:', before_func))
    protected_matches = list(re.finditer(r'\bprotected\s*:', before_func))
    public_matches = list(re.finditer(r'\bpublic\s*:', before_func))

    if not private_matches and not protected_matches and not public_matches:
        return False

    # Get the most recent access specifier
    last_private = private_matches[-1].end() if private_matches else -1
    last_protected = protected_matches[-1].end() if protected_matches else -1
    last_public = public_matches[-1].end() if public_matches else -1

    last_access = max(last_private, last_protected, last_public)

    return last_access == last_private


def is_member_variable(content: str, func_pos: int) -> bool:
    """Check if a matched 'function' is actually a member variable declaration."""
    # Look ahead to see if this is a member variable with a semicolon
    # Get a snippet after the match position
    snippet = content[func_pos:min(func_pos + 200, len(content))]
    # Member variables typically end with ; followed by newline or another declaration
    # They don't have a function body { after the closing parenthesis/brace
    # Look for pattern like: has_val_;  or has_val_ = true;
    if re.search(r';\s*(?://|/\*|\n|$)', snippet):
        # This looks like a member variable, not a function
        return True
    return False


def check_function_blocks(content: str, file: Path) -> List[Violation]:
    violations: List[Violation] = []

    # Use finditer to get both match data and position
    for match in FUNCTION_PATTERN.finditer(content):
        return_type = match.group(1)
        name = match.group(2)
        params = match.group(3)
        func_pos = match.start()

        # Skip keywords that are not functions
        if name in NON_FUNCTION_KEYWORDS:
            continue

        # Skip member variables (they look like: bool has_val_;)
        if is_member_variable(content, func_pos):
            continue

        # Skip private functions
        if is_private_function(content, func_pos):
            continue

        # Use func_pos consistently (from the regex match)
        snippet = content[max(0, func_pos - 1000):func_pos]
        block_match = DOXYGEN_BLOCK_PATTERN.search(snippet)

        if not block_match:
            violations.append(
                Violation(file, "Missing Doxygen block", name)
            )
            continue

        block = block_match.group()

        # Only check core required tags
        for tag in CORE_REQUIRED_TAGS:
            if tag not in block:
                violations.append(
                    Violation(file, f"Missing required tag {tag}", name)
                )

        # Skip @return check for void functions that have explanatory @return
        # (e.g., "Returns void if in success state" - this is documentation, not a type error)
        # Only warn if @return describes an actual type (not just "void" or descriptive text)
        if return_type.strip() == "void" and "@return" in block:
            # Check if @return contains more than just "void" or descriptive text
            # This is okay: "@return     void if in success state."
            # This is NOT okay: "@return     SomeType"
            return_lines = [line for line in block.splitlines() if "@return" in line]
            for line in return_lines:
                # Remove @return and common words to see if there's an actual type mentioned
                cleaned = re.sub(r'@return\s*', '', line)
                cleaned = re.sub(r'\b(void|null|none|nothing)\b', '', cleaned, flags=re.IGNORECASE)
                cleaned = re.sub(r'[^\w]', '', cleaned)
                if cleaned and len(cleaned) > 3:
                    # There's something that looks like a type name
                    violations.append(
                        Violation(file, "Void function contains @return with type", name)
                    )
                    break

        if return_type.strip() != "void" and "@return" not in block:
            violations.append(
                Violation(file, "Non-void function missing @return", name)
            )

        for line in block.splitlines():
            if "@param" in line and not re.search(r"\[(in|out|in,out)\]", line):
                violations.append(
                    Violation(file, "@param missing direction", name)
                )

    return violations


def check_language_rules(content: str, file: Path) -> List[Violation]:
    violations: List[Violation] = []

    if FIRST_PERSON_PATTERN.search(content):
        violations.append(Violation(file, "First-person usage detected"))

    if FUTURE_TENSE_PATTERN.search(content):
        violations.append(Violation(file, "Future tense 'will' detected"))

    return violations


def analyze_file(path: Path) -> List[Violation]:
    content = read_file(path)

    violations: List[Violation] = []
    violations += check_line_length(content, path)
    violations += check_file_header(content, path)
    violations += check_enum_struct_docs(content, path)
    violations += check_function_blocks(content, path)
    violations += check_language_rules(content, path)

    return violations


def collect_files(root: Path) -> Iterable[Path]:
    for path in root.rglob("*"):
        if path.is_file() and not should_skip(path):
            yield path


def write_failed_markdown(violations: List[Violation]) -> None:
    if not violations:
        if FAILED_OUTPUT_FILE.exists():
            FAILED_OUTPUT_FILE.unlink()
        return

    lines: List[str] = [
        "# FAILED DOXYGEN CHECKS",
        "",
        f"Total violations: {len(violations)}",
        "",
    ]

    for violation in violations:
        lines.append(violation.to_markdown())

    FAILED_OUTPUT_FILE.write_text("\n".join(lines), encoding="utf-8")


# ============================================================
# ========================= MAIN =============================
# ============================================================


def main() -> int:
    if len(sys.argv) > 1:
        root = Path(sys.argv[1]).resolve()
    else:
        root = PROJECT_ROOT

    all_violations: List[Violation] = []

    for file_path in collect_files(root):
        all_violations.extend(analyze_file(file_path))

    write_failed_markdown(all_violations)

    if all_violations:
        print(f"❌ FAILED: {len(all_violations)} violations")
        print(f"See: {FAILED_OUTPUT_FILE}")
        return 1

    print("✅ All Doxygen checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())