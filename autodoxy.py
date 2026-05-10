import argparse
import json
import os
import sys
import urllib.error
import urllib.request
from pathlib import Path


DEFAULT_MODEL = "gemini-2.5-flash"
DEFAULT_API_KEY_ENV = "GEMINI_API_KEY"
API_URL_TEMPLATE = (
    "https://generativelanguage.googleapis.com/v1beta/models/"
    "{model}:generateContent"
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate Doxygen-style comments for source code using Gemini."
    )
    parser.add_argument("input_path", help="Path to the input source file.")
    parser.add_argument("output_path", help="Path to write the commented source file.")
    parser.add_argument(
        "--model",
        default=DEFAULT_MODEL,
        help=f"Gemini model to use. Default: {DEFAULT_MODEL}",
    )
    parser.add_argument(
        "--api-key-env",
        default=DEFAULT_API_KEY_ENV,
        help=(
            "Environment variable containing the Gemini API key. "
            f"Default: {DEFAULT_API_KEY_ENV}"
        ),
    )
    parser.add_argument(
        "--env-file",
        default=".env",
        help="Path to a .env file to load before reading the API key. Default: .env",
    )
    parser.add_argument(
        "--language",
        default="English",
        help="Natural language for generated comments. Default: English",
    )
    parser.add_argument(
        "--temperature",
        type=float,
        default=0.1,
        help="Sampling temperature for Gemini. Default: 0.1",
    )
    return parser.parse_args()


def load_dotenv(env_path: Path) -> None:
    if not env_path.exists():
        return

    for raw_line in env_path.read_text(encoding="utf-8").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue

        key, value = line.split("=", 1)
        key = key.strip()
        value = value.strip()

        if not key:
            continue

        if value and value[0] == value[-1] and value[0] in {'"', "'"}:
            value = value[1:-1]

        # Keep already-exported environment variables higher priority.
        os.environ.setdefault(key, value)


def build_prompt(code: str, file_name: str, comment_language: str) -> str:
    return f"""You are a precise code documentation assistant.

Task:
- Add clear Doxygen-style comments to the provided source file.
- Focus on file comments, class comments, function comments, method comments, and important member comments.
- Preserve the program's behavior.
- Do not rename identifiers.
- Do not remove existing code.
- Only make the smallest formatting changes needed to insert comments.
- Keep comments concise and technically accurate.
- Write comments in {comment_language}.
- Return only the full updated source code for the file, with no markdown fences and no extra explanation.

File name: {file_name}

Source code:
{code}
"""


def call_gemini(api_key: str, model: str, prompt: str, temperature: float) -> str:
    request_body = {
        "systemInstruction": {
            "parts": [
                {
                    "text": (
                        "Return only source code. Do not wrap the answer in markdown. "
                        "Do not explain your changes."
                    )
                }
            ]
        },
        "contents": [
            {
                "parts": [
                    {
                        "text": prompt,
                    }
                ]
            }
        ],
        "generationConfig": {
            "temperature": temperature,
        },
    }

    request = urllib.request.Request(
        API_URL_TEMPLATE.format(model=model),
        data=json.dumps(request_body).encode("utf-8"),
        headers={
            "Content-Type": "application/json",
            "x-goog-api-key": api_key,
        },
        method="POST",
    )

    try:
        with urllib.request.urlopen(request) as response:
            payload = json.loads(response.read().decode("utf-8"))
    except urllib.error.HTTPError as exc:
        details = exc.read().decode("utf-8", errors="replace")
        raise RuntimeError(f"Gemini API request failed: HTTP {exc.code}\n{details}") from exc
    except urllib.error.URLError as exc:
        raise RuntimeError(f"Gemini API connection failed: {exc.reason}") from exc

    text_parts = []
    for candidate in payload.get("candidates", []):
        content = candidate.get("content", {})
        for part in content.get("parts", []):
            text = part.get("text")
            if text:
                text_parts.append(text)

    if not text_parts:
        raise RuntimeError(
            "Gemini API returned no text content. Raw response:\n"
            + json.dumps(payload, indent=2)
        )

    return "\n".join(text_parts).strip()


def strip_code_fences(text: str) -> str:
    stripped = text.strip()
    if not stripped.startswith("```"):
        return stripped

    lines = stripped.splitlines()
    if lines and lines[0].startswith("```"):
        lines = lines[1:]
    if lines and lines[-1].strip() == "```":
        lines = lines[:-1]
    return "\n".join(lines).strip()


def detect_newline_style(text: str) -> str:
    if "\r\n" in text:
        return "\r\n"
    return "\n"


def normalize_newlines(text: str, newline: str) -> str:
    normalized = text.replace("\r\n", "\n").replace("\r", "\n")
    return normalized.replace("\n", newline)


def main() -> int:
    args = parse_args()
    load_dotenv(Path(args.env_file))

    api_key = os.environ.get(args.api_key_env)
    if not api_key:
        print(
            (
                f"Missing Gemini API key. Add {args.api_key_env} to {args.env_file} "
                f"or set the {args.api_key_env} environment variable."
            ),
            file=sys.stderr,
        )
        return 1

    input_path = Path(args.input_path)
    output_path = Path(args.output_path)

    code = input_path.read_text(encoding="utf-8")
    newline = detect_newline_style(code)
    prompt = build_prompt(code, input_path.name, args.language)

    commented_code = call_gemini(api_key, args.model, prompt, args.temperature)
    commented_code = strip_code_fences(commented_code)
    commented_code = normalize_newlines(commented_code, newline)

    if code.endswith(("\n", "\r\n")) and not commented_code.endswith(newline):
        commented_code += newline

    output_path.write_text(commented_code, encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
