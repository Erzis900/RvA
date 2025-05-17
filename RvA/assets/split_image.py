import os
from PIL import Image
import sys
import shutil

filename = sys.argv[1]
prefix = sys.argv[2]
width = sys.argv[3]
cleanup = sys.argv[4].lower()

FRAME_WIDTH = int(width)
OUTPUT_FOLDER = "frames_output"

if cleanup == "y" and os.path.exists(OUTPUT_FOLDER):
    print(f"Cleaning up {OUTPUT_FOLDER}...")
    shutil.rmtree(OUTPUT_FOLDER)

def split_image_into_frames(input_path, output_prefix):
    os.makedirs(OUTPUT_FOLDER, exist_ok=True)

    image = Image.open(input_path)
    width, height = image.size
    num_frames = width // FRAME_WIDTH

    for i in range(num_frames):
        left = i * FRAME_WIDTH
        upper = 0
        right = left + FRAME_WIDTH
        lower = height

        frame = image.crop((left, upper, right, lower))
        frame_path = os.path.join(OUTPUT_FOLDER, f"{output_prefix}_frame{i}.png")

        # Remove the file if it already exists
        if os.path.exists(frame_path):
            os.remove(frame_path)

        frame.save(frame_path)

# Example usage
split_image_into_frames(filename, prefix)