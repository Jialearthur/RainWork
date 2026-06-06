import csv
import math
import random
from pathlib import Path


SAMPLE_RATE = 200_000
FRAME_POINTS = 600
OUTPUT_DIR = Path(__file__).resolve().parents[1]
RAW_DIR = OUTPUT_DIR / "raw"
MOCK_DIR = OUTPUT_DIR / "mock"
PLOT_DIR = OUTPUT_DIR / "plots"


def ensure_dirs():
    RAW_DIR.mkdir(parents=True, exist_ok=True)
    MOCK_DIR.mkdir(parents=True, exist_ok=True)
    PLOT_DIR.mkdir(parents=True, exist_ok=True)


def generate_static_frame():
    baseline = 2048
    frame = []
    for _ in range(FRAME_POINTS):
        noise = random.randint(-12, 12)
        frame.append(baseline + noise)
    return frame


def generate_rain_event_frame():
    frame = generate_static_frame()
    center = random.randint(180, 420)
    width = random.randint(12, 40)
    amplitude = random.randint(180, 420)

    for index in range(FRAME_POINTS):
        distance = index - center
        pulse = amplitude * math.exp(-(distance * distance) / (2 * width * width))
        ripple = 20 * math.sin(index / 5.0)
        frame[index] = int(frame[index] + pulse + ripple)

    return frame


def save_csv(path, frame):
    with path.open("w", newline="", encoding="utf-8") as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow(["index", "adc"])
        for index, value in enumerate(frame):
            writer.writerow([index, value])


def peak_to_peak(frame):
    return max(frame) - min(frame)


def mean(frame):
    return sum(frame) / len(frame)


def energy(frame):
    avg = mean(frame)
    return sum(abs(value - avg) for value in frame)


def variance(frame):
    avg = mean(frame)
    return sum((value - avg) ** 2 for value in frame) / len(frame)


def analyze_frame(frame):
    return {
        "peak_to_peak": peak_to_peak(frame),
        "energy": energy(frame),
        "variance": variance(frame),
    }


def load_csv(path):
    values = []
    with path.open("r", encoding="utf-8") as csv_file:
        reader = csv.DictReader(csv_file)
        for row in reader:
            values.append(int(row["adc"]))
    return values


def save_plot_text(path, frame, title):
    minimum = min(frame)
    maximum = max(frame)
    span = max(maximum - minimum, 1)
    rows = []
    step = max(1, len(frame) // 80)

    rows.append(title)
    rows.append(f"min={minimum} max={maximum} span={span}")

    for value in frame[::step]:
        level = int((value - minimum) * 50 / span)
        rows.append("█" * max(level, 1))

    path.write_text("\n".join(rows), encoding="utf-8")


def print_metrics(label, metrics):
    print(f"{label}:")
    print(f"  peak_to_peak = {metrics['peak_to_peak']:.2f}")
    print(f"  energy       = {metrics['energy']:.2f}")
    print(f"  variance     = {metrics['variance']:.2f}")


def main():
    ensure_dirs()

    static_frame = generate_static_frame()
    rain_frame = generate_rain_event_frame()

    static_csv = MOCK_DIR / "static_frame.csv"
    rain_csv = MOCK_DIR / "rain_event_frame.csv"
    static_plot = PLOT_DIR / "static_frame_plot.txt"
    rain_plot = PLOT_DIR / "rain_event_frame_plot.txt"

    save_csv(static_csv, static_frame)
    save_csv(rain_csv, rain_frame)
    save_plot_text(static_plot, static_frame, "Static Frame")
    save_plot_text(rain_plot, rain_frame, "Rain Event Frame")

    static_metrics = analyze_frame(static_frame)
    rain_metrics = analyze_frame(rain_frame)

    print_metrics("Static frame", static_metrics)
    print_metrics("Rain event frame", rain_metrics)
    print()
    print(f"Saved: {static_csv}")
    print(f"Saved: {rain_csv}")
    print(f"Saved: {static_plot}")
    print(f"Saved: {rain_plot}")


if __name__ == "__main__":
    main()
