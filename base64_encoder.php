# base64_encoder.php
#!/usr/bin/env php
<?php

function progressBar($current, $total, $width = 40) {
    if ($total == 0) return;
    $percent = $current / $total;
    $filled = (int)($width * $percent);
    $bar = str_repeat('█', $filled) . str_repeat('░', $width - $filled);
    fprintf(STDERR, "\rProgress: [%s] %.1f%%", $bar, $percent * 100);
    if ($current >= $total) fwrite(STDERR, "\n");
}

function encodeFile($inputPath, $outputPath, $urlSafe) {
    if (!file_exists($inputPath)) {
        fwrite(STDERR, "Error: input file '$inputPath' not found.\n");
        exit(1);
    }
    $fileSize = filesize($inputPath);
    $chunkSize = 1024 * 1024;
    $out = $outputPath ? fopen($outputPath, 'w') : STDOUT;
    $fh = fopen($inputPath, 'rb');
    $processed = 0;
    while (!feof($fh)) {
        $chunk = fread($fh, $chunkSize);
        if ($chunk === false) break;
        $encoded = $urlSafe ? strtr(base64_encode($chunk), '+/', '-_') : base64_encode($chunk);
        fwrite($out, $encoded);
        $processed += strlen($chunk);
        progressBar($processed, $fileSize);
    }
    fclose($fh);
    if ($outputPath) fclose($out);
    echo "\n✅ Encoded '$inputPath' to '".($outputPath ?: 'stdout')."'\n";
}

function decodeFile($inputPath, $outputPath, $urlSafe) {
    if (!file_exists($inputPath)) {
        fwrite(STDERR, "Error: input file '$inputPath' not found.\n");
        exit(1);
    }
    $data = file_get_contents($inputPath);
    if ($data === false) {
        fwrite(STDERR, "Error: failed to read input file.\n");
        exit(1);
    }
    $data = preg_replace('/\s/', '', $data);
    $missing = strlen($data) % 4;
    if ($missing) $data .= str_repeat('=', 4 - $missing);
    if ($urlSafe) {
        $data = strtr($data, '-_', '+/');
    }
    $decoded = base64_decode($data, true);
    if ($decoded === false) {
        fwrite(STDERR, "Error: invalid Base64 input.\n");
        exit(1);
    }
    $out = $outputPath ? fopen($outputPath, 'wb') : STDOUT;
    fwrite($out, $decoded);
    if ($outputPath) fclose($out);
    echo "\n✅ Decoded '$inputPath' to '".($outputPath ?: 'stdout')."'\n";
}

$options = getopt("e:d:o:u", ["encode:", "decode:", "output:", "url-safe", "help"]);
if (isset($options['help']) || (empty($options['e']) && empty($options['encode']) && empty($options['d']) && empty($options['decode']))) {
    echo "Usage: php base64_encoder.php [-e|--encode <file>] [-d|--decode <file>] [-o|--output <file>] [-u|--url-safe]\n";
    exit(0);
}
$encode = $options['e'] ?? $options['encode'] ?? null;
$decode = $options['d'] ?? $options['decode'] ?? null;
$output = $options['o'] ?? $options['output'] ?? null;
$urlSafe = isset($options['u']) || isset($options['url-safe']);

if ($encode) encodeFile($encode, $output, $urlSafe);
elseif ($decode) decodeFile($decode, $output, $urlSafe);
else {
    fwrite(STDERR, "Error: either -e or -d required.\n");
    exit(1);
}
?>
