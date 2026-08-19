// base64_encoder.js
#!/usr/bin/env node
const fs = require('fs');
const path = require('path');

function progressBar(current, total, width = 40) {
    if (total === 0) return;
    const percent = current / total;
    const filled = Math.floor(width * percent);
    const bar = '█'.repeat(filled) + '░'.repeat(width - filled);
    process.stderr.write(`\rProgress: [${bar}] ${(percent * 100).toFixed(1)}%`);
    if (current >= total) process.stderr.write('\n');
}

function encodeFile(inputPath, outputPath, urlSafe) {
    if (!fs.existsSync(inputPath)) {
        console.error(`Error: input file '${inputPath}' not found.`);
        process.exit(1);
    }
    const stats = fs.statSync(inputPath);
    const fileSize = stats.size;
    const chunkSize = 1024 * 1024;
    const out = outputPath ? fs.createWriteStream(outputPath) : process.stdout;
    const stream = fs.createReadStream(inputPath, { highWaterMark: chunkSize });
    let processed = 0;
    stream.on('data', (chunk) => {
        let encoded;
        if (urlSafe) {
            encoded = chunk.toString('base64').replace(/\+/g, '-').replace(/\//g, '_');
        } else {
            encoded = chunk.toString('base64');
        }
        out.write(encoded);
        processed += chunk.length;
        progressBar(processed, fileSize);
    });
    stream.on('end', () => {
        if (outputPath) out.end();
        console.log(`\n✅ Encoded '${inputPath}' to '${outputPath || 'stdout'}'`);
    });
    stream.on('error', (err) => {
        console.error('Error reading file:', err);
        process.exit(1);
    });
}

function decodeFile(inputPath, outputPath, urlSafe) {
    if (!fs.existsSync(inputPath)) {
        console.error(`Error: input file '${inputPath}' not found.`);
        process.exit(1);
    }
    const data = fs.readFileSync(inputPath, 'utf8');
    const clean = data.replace(/[\s]/g, '');
    const missing = clean.length % 4;
    const padded = missing ? clean + '='.repeat(4 - missing) : clean;
    let decoded;
    try {
        if (urlSafe) {
            const standard = padded.replace(/-/g, '+').replace(/_/g, '/');
            decoded = Buffer.from(standard, 'base64');
        } else {
            decoded = Buffer.from(padded, 'base64');
        }
    } catch (e) {
        console.error('Invalid Base64 input:', e.message);
        process.exit(1);
    }
    if (outputPath) {
        fs.writeFileSync(outputPath, decoded);
        console.log(`✅ Decoded '${inputPath}' to '${outputPath}'`);
    } else {
        process.stdout.write(decoded);
        console.log(`\n✅ Decoded '${inputPath}' to stdout`);
    }
}

const args = process.argv.slice(2);
let encode, decode, output, urlSafe = false;
for (let i = 0; i < args.length; i++) {
    switch (args[i]) {
        case '-e': encode = args[++i]; break;
        case '-d': decode = args[++i]; break;
        case '-o': output = args[++i]; break;
        case '-u': urlSafe = true; break;
        case '-h': case '--help':
            console.log(`Usage: node base64_encoder.js -e <file> [-o <out>] [-u]\n       node base64_encoder.js -d <file> [-o <out>] [-u]`);
            process.exit(0);
    }
}
if (!encode && !decode) {
    console.error('Error: either -e or -d required');
    process.exit(1);
}
if (encode) encodeFile(encode, output, urlSafe);
else decodeFile(decode, output, urlSafe);
