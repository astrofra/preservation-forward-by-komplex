#!/usr/bin/env python3

import argparse
import csv
import shutil
import wave
from pathlib import Path


def format_seconds(time_ms: int) -> str:
    return f"{time_ms / 1000.0:.3f}"


def read_manifest_rows(manifest_path: Path):
    with manifest_path.open("r", encoding="ascii", newline="") as handle:
        return list(csv.DictReader(handle))


def read_wav_pcm(wav_path: Path):
    if not wav_path.is_file():
        return None

    with wave.open(str(wav_path), "rb") as handle:
        params = handle.getparams()
        if params.comptype != "NONE":
            return None
        return {
            "sample_rate": params.framerate,
            "channels": params.nchannels,
            "sample_width": params.sampwidth,
            "frame_count": params.nframes,
            "data": handle.readframes(params.nframes),
        }


def write_wav_bytes(wav_path: Path, sample_rate: int, channels: int, sample_width: int, data: bytes):
    with wave.open(str(wav_path), "wb") as handle:
        handle.setnchannels(channels)
        handle.setsampwidth(sample_width)
        handle.setframerate(sample_rate)
        handle.writeframes(data)


def write_silent_wav(wav_path: Path, sample_rate: int, channels: int, sample_width: int, total_frames: int):
    silent_data = b"\x00" * total_frames * channels * sample_width
    write_wav_bytes(wav_path, sample_rate, channels, sample_width, silent_data)


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output-dir", required=True)
    parser.add_argument("--intro-dir", required=True)
    parser.add_argument("--saari-dir", required=True)
    parser.add_argument("--kukot-dir", default="")
    parser.add_argument("--fps", type=int, default=50)
    parser.add_argument("--sample-rate", type=int, default=22050)
    return parser.parse_args()


def main():
    args = parse_args()

    output_dir = Path(args.output_dir)
    frames_dir = output_dir / "frames"
    audio_dir = output_dir / "audio"
    frames_dir.mkdir(parents=True, exist_ok=True)
    audio_dir.mkdir(parents=True, exist_ok=True)

    for child in frames_dir.iterdir():
        if child.is_file():
            child.unlink()
    for child in audio_dir.iterdir():
        if child.is_file():
            child.unlink()

    segments = [
        {"name": "intro", "dir": Path(args.intro_dir)},
        {"name": "saari", "dir": Path(args.saari_dir)},
    ]
    if args.kukot_dir.strip():
        segments.append({"name": "kukot", "dir": Path(args.kukot_dir)})

    segment_rows = {}
    for segment in segments:
        segment_rows[segment["name"]] = read_manifest_rows(segment["dir"] / "manifest.csv")

    manifest_path = output_dir / "manifest.csv"
    with manifest_path.open("w", encoding="ascii", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow([
            "capture_index",
            "render_frame",
            "demo_time_ms",
            "demo_time_seconds",
            "scene_time_ms",
            "scene_time_seconds",
            "scene",
            "next_script_time_hex",
            "frame_path",
        ])

        frame_index = 0
        segment_start_time_ms = 0

        for segment in segments:
            rows = segment_rows[segment["name"]]
            for row in rows:
                source_frame = segment["dir"] / row["frame_path"]
                target_name = f"frame_{frame_index:06d}.tga"
                target_frame = frames_dir / target_name
                shutil.copyfile(source_frame, target_frame)

                demo_time_ms = segment_start_time_ms + int(row["demo_time_ms"])
                scene_time_ms = int(row["scene_time_ms"])
                writer.writerow([
                    frame_index,
                    frame_index,
                    demo_time_ms,
                    format_seconds(demo_time_ms),
                    scene_time_ms,
                    format_seconds(scene_time_ms),
                    row["scene"],
                    row["next_script_time_hex"],
                    f"frames/{target_name}",
                ])
                frame_index += 1

            if rows:
                last_row = rows[-1]
                segment_start_time_ms += int(last_row["demo_time_ms"]) + int(round(1000.0 / args.fps))

    samples_per_frame = args.sample_rate // args.fps
    wav_path = audio_dir / "forward.wav"

    segment_audio = []
    audio_compatible = True
    for segment in segments:
        audio = read_wav_pcm(segment["dir"] / "audio" / "forward.wav")
        if audio is None:
            audio_compatible = False
            break
        if segment_audio:
            reference = segment_audio[0]
            if (audio["sample_rate"] != reference["sample_rate"] or
                    audio["channels"] != reference["channels"] or
                    audio["sample_width"] != reference["sample_width"]):
                audio_compatible = False
                break
        segment_audio.append(audio)

    if audio_compatible and segment_audio:
        reference = segment_audio[0]
        merged_audio = b"".join(audio["data"] for audio in segment_audio)
        write_wav_bytes(
            wav_path,
            reference["sample_rate"],
            reference["channels"],
            reference["sample_width"],
            merged_audio,
        )
    else:
        write_silent_wav(wav_path, args.sample_rate, 2, 2, frame_index * samples_per_frame)

    log_path = output_dir / "log.txt"
    with log_path.open("w", encoding="ascii", newline="") as handle:
        segment_names = ",".join(segment["name"] for segment in segments)
        handle.write("forward-export current full wrapper\n")
        handle.write("resolution=512x256\n")
        handle.write(f"fps={args.fps}\n")
        handle.write(f"sample_rate={args.sample_rate}\n")
        handle.write(f"samples_per_frame={samples_per_frame}\n")
        handle.write("sequence=current-full\n")
        handle.write(f"segments={segment_names}\n")
        handle.write(f"frames={frame_index}\n")
        handle.write(
            "note=merged wrapper output for all currently ported non-placeholder sequences; "
            "later demo scenes remain unported\n"
        )
        for segment in segments:
            handle.write(f"{segment['name']}_frames={len(segment_rows[segment['name']])}\n")


if __name__ == "__main__":
    main()
