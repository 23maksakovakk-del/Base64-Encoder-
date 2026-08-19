📦 Base64 Encoder — Multi‑Language File Encoder/Decoder
8 languages, one powerful tool – encode or decode any file to/from Base64 with streaming, progress, and URL‑safe support.

✨ Features
🔐 Encode any file to Base64 (text or binary)

🔓 Decode Base64 back to original file

📁 Supports large files via streaming (chunked processing)

🌐 URL‑safe alphabet option (base64url)

📊 Progress bar for large files (where supported)

📤 Output to file or stdout (auto‑detect)

✅ Validation and error checking

🖥️ Cross‑platform CLI interface

🧰 Supported Languages & Dependencies
Language	File	Dependencies (stdlib only)
Python	base64_encoder.py	none (uses base64, argparse)
Go	base64_encoder.go	none (uses encoding/base64)
JavaScript (Node)	base64_encoder.js	none (uses fs, path)
Ruby	base64_encoder.rb	none (uses base64)
PHP	base64_encoder.php	none (uses built‑in functions)
Java	Base64Encoder.java	Java 8+ (uses java.util.Base64)
C#	Base64Encoder.cs	.NET Core 3.1+ (uses System.Convert)
C++	base64_encoder.cpp	none (self‑contained base64 implementation)
🚀 Common Usage
All implementations follow the same CLI pattern:

bash
# Encode a file to Base64
<command> -e input.txt -o output.b64

# Decode a Base64 file back to original
<command> -d output.b64 -o restored.txt

# Use URL‑safe alphabet (Base64URL)
<command> -e input.bin -o encoded.b64 --url-safe

# Pipe to stdout (no -o)
<command> -e input.jpg > encoded.b64

# Decode from stdin and write to file
cat encoded.b64 | <command> -d -o decoded.jpg
Arguments:

-e, --encode <file> – encode the given file

-d, --decode <file> – decode the given file (must be valid Base64)

-o, --output <file> – output file (if omitted, prints to stdout)

-u, --url-safe – use URL‑safe Base64 alphabet (-_ instead of +/)

-h, --help – show help

📸 Example Output (Encode)
text
📦 Base64 Encoder
Encoding: input.txt → output.b64
Progress: [████████████████████████] 100%
Done.
📁 Repository Structure
text
.
├── README.md
├── python/
│   └── base64_encoder.py
├── go/
│   └── base64_encoder.go
├── javascript/
│   └── base64_encoder.js
├── ruby/
│   └── base64_encoder.rb
├── php/
│   └── base64_encoder.php
├── java/
│   └── Base64Encoder.java
├── csharp/
│   └── Base64Encoder.cs
└── cpp/
    └── base64_encoder.cpp
