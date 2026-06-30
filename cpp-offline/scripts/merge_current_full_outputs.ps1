param(
    [Parameter(Mandatory = $true)]
    [string]$OutputDir,

    [Parameter(Mandatory = $true)]
    [string]$IntroDir,

    [Parameter(Mandatory = $true)]
    [string]$SaariDir,

    [int]$Fps = 50,
    [int]$SampleRate = 22050
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$Culture = [System.Globalization.CultureInfo]::InvariantCulture

function Ensure-Directory {
    param([string]$Path)

    if (-not (Test-Path -LiteralPath $Path)) {
        New-Item -ItemType Directory -Path $Path | Out-Null
    }
}

function Format-Seconds {
    param([long]$TimeMs)

    return ($TimeMs / 1000.0).ToString("0.000", $Culture)
}

function Escape-Csv {
    param([AllowNull()][string]$Value)

    if ($null -eq $Value) {
        return ""
    }

    if ($Value.Contains(",") -or $Value.Contains('"')) {
        return '"' + $Value.Replace('"', '""') + '"'
    }

    return $Value
}

function Write-SilentWav {
    param(
        [string]$Path,
        [int]$SampleRateValue,
        [int]$Channels,
        [int]$BitsPerSample,
        [long]$TotalSamples
    )

    $bytesPerSampleFrame = $Channels * ($BitsPerSample / 8)
    $dataSize = [long]$TotalSamples * [long]$bytesPerSampleFrame
    $riffSize = 36 + $dataSize
    $byteRate = $SampleRateValue * $bytesPerSampleFrame
    $blockAlign = $bytesPerSampleFrame

    $fileStream = [System.IO.File]::Create($Path)
    try {
        $writer = New-Object System.IO.BinaryWriter($fileStream)
        try {
            $writer.Write([System.Text.Encoding]::ASCII.GetBytes("RIFF"))
            $writer.Write([int]$riffSize)
            $writer.Write([System.Text.Encoding]::ASCII.GetBytes("WAVE"))
            $writer.Write([System.Text.Encoding]::ASCII.GetBytes("fmt "))
            $writer.Write([int]16)
            $writer.Write([int16]1)
            $writer.Write([int16]$Channels)
            $writer.Write([int]$SampleRateValue)
            $writer.Write([int]$byteRate)
            $writer.Write([int16]$blockAlign)
            $writer.Write([int16]$BitsPerSample)
            $writer.Write([System.Text.Encoding]::ASCII.GetBytes("data"))
            $writer.Write([int]$dataSize)

            $zeroChunk = New-Object byte[] 8192
            $remainingBytes = $dataSize
            while ($remainingBytes -gt 0) {
                $chunkSize = [int][Math]::Min($zeroChunk.Length, $remainingBytes)
                $writer.Write($zeroChunk, 0, $chunkSize)
                $remainingBytes -= $chunkSize
            }
        } finally {
            $writer.Dispose()
        }
    } finally {
        $fileStream.Dispose()
    }
}

Ensure-Directory -Path $OutputDir
$framesDir = Join-Path $OutputDir "frames"
$audioDir = Join-Path $OutputDir "audio"
Ensure-Directory -Path $framesDir
Ensure-Directory -Path $audioDir

Get-ChildItem -LiteralPath $framesDir -File -ErrorAction SilentlyContinue | Remove-Item -Force
Get-ChildItem -LiteralPath $audioDir -File -ErrorAction SilentlyContinue | Remove-Item -Force

$introManifestPath = Join-Path $IntroDir "manifest.csv"
$saariManifestPath = Join-Path $SaariDir "manifest.csv"
$introRows = @(Import-Csv -LiteralPath $introManifestPath)
$saariRows = @(Import-Csv -LiteralPath $saariManifestPath)

$manifestPath = Join-Path $OutputDir "manifest.csv"
$manifestWriter = New-Object System.IO.StreamWriter($manifestPath, $false, [System.Text.Encoding]::ASCII)

try {
    $manifestWriter.WriteLine("capture_index,render_frame,demo_time_ms,demo_time_seconds,scene_time_ms,scene_time_seconds,scene,next_script_time_hex,frame_path")

    $frameIndex = 0
    $introDurationMs = 0L
    if ($introRows.Count -gt 0) {
        $lastIntro = $introRows[$introRows.Count - 1]
        $introDurationMs = [long]$lastIntro.demo_time_ms + [long][Math]::Round(1000.0 / $Fps)
    }

    foreach ($row in $introRows) {
        $sourceFrame = Join-Path $IntroDir ($row.frame_path -replace "/", "\")
        $targetName = ("frame_{0:D6}.tga" -f $frameIndex)
        $targetFrame = Join-Path $framesDir $targetName
        Copy-Item -LiteralPath $sourceFrame -Destination $targetFrame -Force

        $demoTimeMs = [long]$row.demo_time_ms
        $sceneTimeMs = [long]$row.scene_time_ms
        $framePath = "frames/$targetName"
        $manifestWriter.WriteLine((
            "{0},{1},{2},{3},{4},{5},{6},{7},{8}" -f
            $frameIndex,
            $frameIndex,
            $demoTimeMs,
            (Format-Seconds -TimeMs $demoTimeMs),
            $sceneTimeMs,
            (Format-Seconds -TimeMs $sceneTimeMs),
            (Escape-Csv -Value ([string]$row.scene)),
            (Escape-Csv -Value ([string]$row.next_script_time_hex)),
            (Escape-Csv -Value $framePath)
        ))

        $frameIndex += 1
    }

    foreach ($row in $saariRows) {
        $sourceFrame = Join-Path $SaariDir ($row.frame_path -replace "/", "\")
        $targetName = ("frame_{0:D6}.tga" -f $frameIndex)
        $targetFrame = Join-Path $framesDir $targetName
        Copy-Item -LiteralPath $sourceFrame -Destination $targetFrame -Force

        $demoTimeMs = $introDurationMs + [long]$row.demo_time_ms
        $sceneTimeMs = [long]$row.scene_time_ms
        $framePath = "frames/$targetName"
        $manifestWriter.WriteLine((
            "{0},{1},{2},{3},{4},{5},{6},{7},{8}" -f
            $frameIndex,
            $frameIndex,
            $demoTimeMs,
            (Format-Seconds -TimeMs $demoTimeMs),
            $sceneTimeMs,
            (Format-Seconds -TimeMs $sceneTimeMs),
            (Escape-Csv -Value ([string]$row.scene)),
            (Escape-Csv -Value ([string]$row.next_script_time_hex)),
            (Escape-Csv -Value $framePath)
        ))

        $frameIndex += 1
    }
} finally {
    $manifestWriter.Dispose()
}

$samplesPerFrame = [int]($SampleRate / $Fps)
$totalSamples = [long]$frameIndex * [long]$samplesPerFrame
$wavPath = Join-Path $audioDir "forward.wav"
Write-SilentWav -Path $wavPath -SampleRateValue $SampleRate -Channels 2 -BitsPerSample 16 -TotalSamples $totalSamples

$logPath = Join-Path $OutputDir "log.txt"
$logLines = @(
    "forward-export current full wrapper",
    "resolution=512x256",
    "fps=$Fps",
    "sample_rate=$SampleRate",
    "samples_per_frame=$samplesPerFrame",
    "sequence=current-full",
    "segments=intro,saari",
    "intro_frames=$($introRows.Count)",
    "saari_frames=$($saariRows.Count)",
    "frames=$frameIndex",
    "note=merged wrapper output for all currently ported non-placeholder sequences; remaining demo scenes are still unported"
)
[System.IO.File]::WriteAllLines($logPath, $logLines, [System.Text.Encoding]::ASCII)
