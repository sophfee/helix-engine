"""
I like python for writing scripts over Powershell or a shell script.
"""

import os
import sys
import subprocess
from os.path import join, dirname, abspath, getmtime, exists, splitext, basename
from subprocess import Popen, PIPE

COMPILED_FILES = []

def compress_texture(file):
	global COMPILED_FILES
	# Check if the file has already been compiled
	if file in COMPILED_FILES:
		return

	# Check if the file exists
	if not os.path.exists(file):
		print(f"File {file} does not exist.")
		return

	# Determine the output file path (change extension to .ktx)
	output_file = os.path.splitext(file)[0] + ".dds"

	# Check if the output file exists and its last modified time
	# if os.path.exists(output_file):
	# 	output_last_modified = os.path.getmtime(output_file)
	# 	if output_last_modified >= source_last_modified:
	# 		print(f"Skipping {file}, output is up to date.")
	# 		return

	# Run the compression command (using a hypothetical tool 'texture-compressor')
	command = ["nvtt_export.exe", file, "-f", "15", "-o", output_file]
	print(f"Compressing {file} to {output_file}...")
	subprocess.run(command)

	# Add the file to the list of compiled files
	COMPILED_FILES.append(file)
	
def walk_directory(directory: str):
	for root, dirs, files in os.walk(directory, topdown=True, onerror=None, followlinks=False):
		for name in files:
			if name.endswith(".png"):
				compress_texture(os.path.join(root, name))
		for name in dirs:
			walk_directory(os.path.join(root, name))

if __name__ == "__main__":
	walk_directory(os.path.join(os.path.dirname(__file__), "test-resources"))
	sys.stdout.flush()