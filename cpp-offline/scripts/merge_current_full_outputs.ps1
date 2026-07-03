param(
    [Parameter(Mandatory = $true)]
    [string]$OutputDir,

    [Parameter(Mandatory = $true)]
    [string]$IntroDir,

    [Parameter(Mandatory = $true)]
    [string]$SaariDir,

    [string]$KukotDir = "",
    [string]$MakuDir = "",

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

function Write-WavBytes {
    param(
        [string]$Path,
        [int]$SampleRateValue,
        [int]$Channels,
        [int]$BitsPerSample,
        [byte[]]$Data
    )

    $dataSize = if ($null -eq $Data) { 0L } else { [long]$Data.Length }
    $bytesPerSampleFrame = $Channels * ($BitsPerSample / 8)
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
            if ($dataSize -gt 0) {
                $writer.Write($Data)
            }
        } finally {
            $writer.Dispose()
        }
    } finally {
        $fileStream.Dispose()
    }
}

function Read-WavPcm {
    param([string]$Path)

    if (-not (Test-Path -LiteralPath $Path)) {
        return $null
    }

    $fileStream = [System.IO.File]::OpenRead($Path)
    try {
        $reader = New-Object System.IO.BinaryReader($fileStream)
        try {
            $riff = [System.Text.Encoding]::ASCII.GetString($reader.ReadBytes(4))
            if ($riff -ne "RIFF") {
                return $null
            }
            [void]$reader.ReadUInt32()
            $wave = [System.Text.Encoding]::ASCII.GetString($reader.ReadBytes(4))
            if ($wave -ne "WAVE") {
                return $null
            }

            $sampleRateValue = 0
            $channels = 0
            $bitsPerSample = 0
            $data = $null

            while ($fileStream.Position -lt $fileStream.Length) {
                $chunkIdBytes = $reader.ReadBytes(4)
                if ($chunkIdBytes.Length -lt 4) {
                    break
                }

                $chunkId = [System.Text.Encoding]::ASCII.GetString($chunkIdBytes)
                $chunkSize = [int]$reader.ReadUInt32()

                if ($chunkId -eq "fmt ") {
                    $audioFormat = [int]$reader.ReadUInt16()
                    $channels = [int]$reader.ReadUInt16()
                    $sampleRateValue = [int]$reader.ReadUInt32()
                    [void]$reader.ReadUInt32()
                    [void]$reader.ReadUInt16()
                    $bitsPerSample = [int]$reader.ReadUInt16()
                    if ($audioFormat -ne 1) {
                        return $null
                    }
                    if ($chunkSize -gt 16) {
                        [void]$fileStream.Seek($chunkSize - 16, [System.IO.SeekOrigin]::Current)
                    }
                } elseif ($chunkId -eq "data") {
                    $data = $reader.ReadBytes($chunkSize)
                    if (($chunkSize % 2) -eq 1) {
                        [void]$fileStream.Seek(1, [System.IO.SeekOrigin]::Current)
                    }
                    break
                } else {
                    [void]$fileStream.Seek($chunkSize + ($chunkSize % 2), [System.IO.SeekOrigin]::Current)
                }
            }

            if ($null -eq $data -or $sampleRateValue -le 0 -or $channels -le 0 -or $bitsPerSample -le 0) {
                return $null
            }

            return [PSCustomObject]@{
                SampleRate = $sampleRateValue
                Channels = $channels
                BitsPerSample = $bitsPerSample
                Data = $data
            }
        } finally {
            $reader.Dispose()
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

$segments = @(
    [PSCustomObject]@{ Name = "intro"; Dir = $IntroDir },
    [PSCustomObject]@{ Name = "saari"; Dir = $SaariDir }
)
if (-not [string]::IsNullOrWhiteSpace($KukotDir)) {
    $segments += [PSCustomObject]@{ Name = "kukot"; Dir = $KukotDir }
}
if (-not [string]::IsNullOrWhiteSpace($MakuDir)) {
    $segments += [PSCustomObject]@{ Name = "maku"; Dir = $MakuDir }
}

$segmentRows = @{}
foreach ($segment in $segments) {
    $manifestPath = Join-Path $segment.Dir "manifest.csv"
    $segmentRows[$segment.Name] = @(Import-Csv -LiteralPath $manifestPath)
}

$manifestPath = Join-Path $OutputDir "manifest.csv"
$manifestWriter = New-Object System.IO.StreamWriter($manifestPath, $false, [System.Text.Encoding]::ASCII)

try {
    $manifestWriter.WriteLine("capture_index,render_frame,demo_time_ms,demo_time_seconds,scene_time_ms,scene_time_seconds,scene,next_script_time_hex,frame_path")

    $frameIndex = 0
    $segmentStartTimeMs = 0L

    foreach ($segment in $segments) {
        $rows = $segmentRows[$segment.Name]
        foreach ($row in $rows) {
            $sourceFrame = Join-Path $segment.Dir ($row.frame_path -replace "/", "\")
            $targetName = ("frame_{0:D6}.tga" -f $frameIndex)
            $targetFrame = Join-Path $framesDir $targetName
            Copy-Item -LiteralPath $sourceFrame -Destination $targetFrame -Force

            $demoTimeMs = $segmentStartTimeMs + [long]$row.demo_time_ms
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

        if ($rows.Count -gt 0) {
            $lastRow = $rows[$rows.Count - 1]
            $segmentStartTimeMs += [long]$lastRow.demo_time_ms + [long][Math]::Round(1000.0 / $Fps)
        }
    }
} finally {
    $manifestWriter.Dispose()
}

$samplesPerFrame = [int]($SampleRate / $Fps)
$wavPath = Join-Path $audioDir "forward.wav"
$segmentAudio = @()
$audioCompatible = $true
foreach ($segment in $segments) {
    $audio = Read-WavPcm -Path (Join-Path (Join-Path $segment.Dir "audio") "forward.wav")
    if ($null -eq $audio) {
        $audioCompatible = $false
        break
    }

    if ($segmentAudio.Count -gt 0) {
        $reference = $segmentAudio[0]
        if ($audio.SampleRate -ne $reference.SampleRate -or
            $audio.Channels -ne $reference.Channels -or
            $audio.BitsPerSample -ne $reference.BitsPerSample) {
            $audioCompatible = $false
            break
        }
    }

    $segmentAudio += $audio
}

if ($audioCompatible -and $segmentAudio.Count -gt 0) {
    $mergedLength = 0
    foreach ($audio in $segmentAudio) {
        $mergedLength += $audio.Data.Length
    }

    $mergedAudio = New-Object byte[] $mergedLength
    $copyOffset = 0
    foreach ($audio in $segmentAudio) {
        [System.Buffer]::BlockCopy($audio.Data, 0, $mergedAudio, $copyOffset, $audio.Data.Length)
        $copyOffset += $audio.Data.Length
    }

    $reference = $segmentAudio[0]
    Write-WavBytes -Path $wavPath `
        -SampleRateValue $reference.SampleRate `
        -Channels $reference.Channels `
        -BitsPerSample $reference.BitsPerSample `
        -Data $mergedAudio
}
else {
    $totalSamples = [long]$frameIndex * [long]$samplesPerFrame
    Write-SilentWav -Path $wavPath -SampleRateValue $SampleRate -Channels 2 -BitsPerSample 16 -TotalSamples $totalSamples
}

$logPath = Join-Path $OutputDir "log.txt"
$segmentNames = ($segments | ForEach-Object { $_.Name }) -join ","
$logLines = @(
    "forward-export current full wrapper",
    "resolution=512x256",
    "fps=$Fps",
    "sample_rate=$SampleRate",
    "samples_per_frame=$samplesPerFrame",
    "sequence=current-full",
    "segments=$segmentNames",
    "frames=$frameIndex",
    "note=merged wrapper output for all currently ported non-placeholder sequences; later demo scenes remain unported"
)
[System.Collections.Generic.List[string]]$frameLines = @()
foreach ($segment in $segments) {
    $frameLines.Add(("{0}_frames={1}" -f $segment.Name, $segmentRows[$segment.Name].Count))
}
$logLines += $frameLines
[System.IO.File]::WriteAllLines($logPath, $logLines, [System.Text.Encoding]::ASCII)
