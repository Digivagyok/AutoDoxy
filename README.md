# AutoDoxy

AutoDoxy is a small Python CLI tool that uses the Gemini API to add Doxygen-style comments to source files.

It reads a source file, sends the code to Gemini with instructions to document it, and writes the commented version to a separate output file. The script is designed to stay lightweight: it uses only the Python standard library and reads the API key from a local `.env` file by default.

## What It Does

- Adds Doxygen-style documentation to existing code
- Preserves the original file's newline style
- Loads the Gemini API key from a `.env` file or the shell environment
- Lets you choose the Gemini model, comment language, and temperature
- Writes the result to a new output file

## Requirements

- Python 3.9 or newer
- A Gemini API key
- Internet access when running the script

## Project Files

- [autodoxy.py](autodoxy.py): main CLI script
- [encrypted_string.h](examples/encrypted_string.h): example input file
- [out.h](examples/out.h): example generated output
- [.env.example](.env.example): example environment file

## Setup

1. Create a `.env` file in the project directory.
2. Add your Gemini API key:

```env
GEMINI_API_KEY=your_api_key_here
```

3. Make sure Python is available:

```bash
python3 --version
```

## Basic Usage

Run the script with an input file and an output file:

```bash
python3 autodoxy.py encrypted_string.h out.h
```

This reads `encrypted_string.h`, asks Gemini to add Doxygen-style comments, and writes the result to `out.h`.

## Command-Line Options

```text
usage: autodoxy.py [-h] [--model MODEL] [--api-key-env API_KEY_ENV]
                   [--env-file ENV_FILE] [--language LANGUAGE]
                   [--temperature TEMPERATURE]
                   input_path output_path
```

### Positional arguments

- `input_path`: path to the source file you want documented
- `output_path`: path where the documented file will be written

### Optional arguments

- `--model`: Gemini model name to use
  - Default: `gemini-2.5-flash`
- `--api-key-env`: environment variable name that stores the API key
  - Default: `GEMINI_API_KEY`
- `--env-file`: path to the `.env` file to load before reading the API key
  - Default: `.env`
- `--language`: natural language used in the generated comments
  - Default: `English`
- `--temperature`: generation temperature
  - Default: `0.1`

## Examples

Generate English comments with the default model:

```bash
python3 autodoxy.py encrypted_string.h out.h
```

Generate Hungarian comments:

```bash
python3 autodoxy.py encrypted_string.h out.h --language Hungarian
```

Use a different `.env` file:

```bash
python3 autodoxy.py encrypted_string.h out.h --env-file config/dev.env
```

Use a different Gemini model:

```bash
python3 autodoxy.py encrypted_string.h out.h --model gemini-2.5-pro
```

Tune output creativity slightly:

```bash
python3 autodoxy.py encrypted_string.h out.h --temperature 0.2
```

## How It Works

1. The script reads the input file as UTF-8 text.
2. It loads variables from the configured `.env` file if present.
3. It reads the Gemini API key from the configured environment variable.
4. It sends the source code and documentation instructions to Gemini's `generateContent` endpoint.
5. It strips accidental Markdown code fences if the model includes them.
6. It normalizes newline style and writes the final code to the output file.

## Notes and Limitations

- The script sends the full source file to an external API. Do not use it on confidential code unless that is acceptable for your project.
- Generated comments are AI-produced and should be reviewed before committing.
- The model may sometimes reformat code slightly while inserting comments.
- The tool currently processes one file at a time.
- The tool writes to a separate output file; it does not modify files in place.

## Troubleshooting

### `Missing Gemini API key`

Make sure one of these is true:

- your `.env` file exists and contains `GEMINI_API_KEY=...`
- or your shell environment already has that variable set

If you use a different variable name, pass it explicitly:

```bash
python3 autodoxy.py encrypted_string.h out.h --api-key-env MY_CUSTOM_KEY_NAME
```

### HTTP errors from Gemini

Common causes:

- invalid API key
- no network access
- wrong model name
- API quota or permission issues

### Output looks wrong

Try one or more of these:

- lower the temperature
- switch to a stronger model
- review and edit the generated file manually
- run the tool on smaller files first

## Development Notes

The script currently has no third-party dependencies, so there is no install step beyond having Python available.

To inspect the CLI help:

```bash
python3 autodoxy.py --help
```

To verify the script syntax locally:

```bash
python3 -m py_compile autodoxy.py
```

## Suggested Workflow

1. Keep the original file unchanged.
2. Generate documentation into a separate output file.
3. Compare the original and generated versions.
4. Review the comments for technical accuracy.
5. Copy the approved changes into your real project files.

## License

No license file is included in this repository yet. Add one if you plan to share or publish the project.
