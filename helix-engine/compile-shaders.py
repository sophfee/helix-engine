"""
I like python for writing scripts over Powershell or a shell script.
"""

import os
import sys
import subprocess
import time
from watchdog.events import FileSystemEvent, FileSystemEventHandler
from watchdog.observers import Observer
from os.path import join, dirname, abspath, getmtime, exists, splitext, basename
from subprocess import Popen, PIPE

SLANGC_PATH = "%VULKAN_SDK%\\Bin\\slangc.exe"
COMPILE_DEBUG_SYMBOLS = True
COMPILED_FILES = []

class Slang:
	def __init__(self, file):
		self._src = file
		self.vk_use_scalar_layout = False
		self.matrix_layout = "column-major"
		self.spv_reflect = False
		self.debug_level = 3	
		self.output = os.path.splitext(file)[0] + ".spv"

	@property
	def parameters(self):
		p = []
		#if self.vk_use_scalar_layout:
		#	p.append("-fvk-use-scalar-layout")
#
		#if self.spv_reflect:
		#	p.append("-spv-reflect")
#
		if self.debug_level > 0:
			p.extend([f"-g{self.debug_level}"])
#
		p.append(f"-matrix-layout-{self.matrix_layout}")
		p.extend(["-profile", "spirv_1_6", "-o", self.output])
		return p

	@property
	def source(self):
		return self._src

	def compile(self):
		global COMPILED_FILES
		resolved_path = os.getenv("VULKAN_SDK") + "\\Bin\\slangc.exe"
		popen_arguments = [resolved_path, self.source] + self.parameters
		#print(popen_arguments)
		subprocess.run(popen_arguments)
		COMPILED_FILES.append(self.source)


class Target:
	def __init__(self, file):
		no_file_extention = os.path.splitext(file)[0]
		self.original_path = file
		self.complete_path = os.path.relpath(file, __file__)
		self.filename = os.path.basename(file)
		self.filedir = os.path.dirname(file)
		self.slang_last_modified = os.path.getmtime(file) if os.path.exists(file) else 0
		self.spirv_last_modified = os.path.getmtime(no_file_extention + ".spv") if os.path.exists(no_file_extention + ".spv") else 0

	@property
	def should_recompile(self):
		return self.slang_last_modified > self.spirv_last_modified

	def compile(self):
		global COMPILED_FILES
		global COMPILE_DEBUG_SYMBOLS

		#if self.complete_path in COMPILED_FILES:
		#	return

		compiler = Slang(self.original_path)
		compiler.debug_level = 0 # 3 if COMPILE_DEBUG_SYMBOLS else 0
		compiler.vk_use_scalar_layout = True
		compiler.compile()

		sys.stdout.write(f"Compiled {os.path.basename(self.original_path)} to {os.path.splitext(os.path.basename(self.original_path))[0] + '.spv'}")
		if COMPILE_DEBUG_SYMBOLS:
			sys.stdout.write(" with debug symbols.")
		sys.stdout.write("\n")
		

	def compile_if_necessary(self):
		global COMPILED_FILES
		path = os.path.join(self.filedir, self.filename)
		if path in COMPILED_FILES:
			return
		self.compile()
		#if self.should_recompile:
		#else:
		#	sys.stdout.write(f"{self.filename} is up to date.\n")

def walk_directory(directory: str):
	for root, dirs, files in os.walk(directory, topdown=True, onerror=None, followlinks=False):
		for name in files:
			if name.endswith(".slang"):
				Target(os.path.join(root, name)).compile_if_necessary()
		for name in dirs:
			walk_directory(os.path.join(root, name))

class SlangEventHandler(FileSystemEventHandler):
	def on_modified(self, event: FileSystemEvent):
		if not event.is_directory and event.src_path.endswith(".slang"):
			Target(event.src_path).compile()

if __name__ == "__main__":
	walk_directory(os.path.dirname(__file__))
	event_handler = SlangEventHandler()
	observer = Observer()
	observer.schedule(event_handler, os.path.dirname(__file__), recursive=True)
	observer.start()

	try:
		while True:
			time.sleep(1)
			sys.stdout.flush()
	except KeyboardInterrupt:
		observer.stop()
	observer.join()

	sys.stdout.flush()