# base64_encoder.py
import sys
import os
import argparse
import base64
import time
from typing import Optional

def progress_bar(current, total, width=40):
    if total == 0:
        return
    percent = current / total
    filled = int(width * percent)
    bar = '█' * filled + '░' * (width - filled)
    sys.stderr.write(f'\rProgress: [{bar}] {percent*100:.1f}%')
    if current >= total:
        sys.stderr.write('\n')

def encode_file(input_path: str, output_path: Optional[str], url_safe: bool = False):
    if not os.path.exists(input_path):
        print(f"Error: input file '{input_path}' not found.", file=sys.stderr)
        sys.exit(1)
    file_size = os.path.getsize(input_path)
    chunk_size = 1024 * 1024  # 1 MB
    out = open(output_path, 'w') if output_path else sys.stdout
    encoder = base64.urlsafe_b64encode if url_safe else base64.b64encode
    processed = 0
    with open(input_path, 'rb') as f:
        while True:
            chunk = f.read(chunk_size)
            if not chunk:
                break
            encoded = encoder(chunk).decode('ascii')
            out.write(encoded)
            processed += len(chunk)
            progress_bar(processed, file_size)
    if output_path:
        out.close()
    print(f"\n✅ Encoded '{input_path}' to '{output_path or 'stdout'}'")

def decode_file(input_path: str, output_path: Optional[str], url_safe: bool = False):
    if not os.path.exists(input_path):
        print(f"Error: input file '{input_path}' not found.", file=sys.stderr)
        sys.exit(1)
    with open(input_path, 'r') as f:
        data = f.read()
    # Remove whitespace and newlines
    data = ''.join(data.split())
    # Add padding if needed
    missing_padding = len(data) % 4
    if missing_padding:
        data += '=' * (4 - missing_padding)
    try:
        decoder = base64.urlsafe_b64decode if url_safe else base64.b64decode
        decoded = decoder(data)
    except Exception as e:
        print(f"Error: invalid Base64 input - {e}", file=sys.stderr)
        sys.exit(1)
    out = open(output_path, 'wb') if output_path else sys.stdout.buffer
    out.write(decoded)
    if output_path:
        out.close()
    print(f"✅ Decoded '{input_path}' to '{output_path or 'stdout'}'")

def main():
    parser = argparse.ArgumentParser(description="Base64 Encoder/Decoder")
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('-e', '--encode', help='Encode file')
    group.add_argument('-d', '--decode', help='Decode file')
    parser.add_argument('-o', '--output', help='Output file (default: stdout)')
    parser.add_argument('-u', '--url-safe', action='store_true', help='Use URL-safe alphabet')
    args = parser.parse_args()
    if args.encode:
        encode_file(args.encode, args.output, args.url_safe)
    elif args.decode:
        decode_file(args.decode, args.output, args.url_safe)

if __name__ == '__main__':
    main()
