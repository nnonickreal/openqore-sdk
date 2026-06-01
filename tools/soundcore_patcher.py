#!/usr/bin/env python3
import os
import sys

# Configuration
EXTRACT_SRC = "../../tools/firmware.bin"
EXTRACT_DST = "../../tools/soundcore_bootloader.bin"
BOOTLOADER_SIZE = 0x18004

def extract_bootloader():
    if not os.path.exists(EXTRACT_SRC):
        print(f"[error] '{EXTRACT_SRC}' not found in the current directory.")
        print(f"[error] cannot extract bootloader. please place '{EXTRACT_SRC}' here.")
        sys.exit(1)

    try:
        print(f"extracting first 0x{BOOTLOADER_SIZE:x} bytes from '{EXTRACT_SRC}'...")
        with open(EXTRACT_SRC, "rb") as f_in:
            data = f_in.read(BOOTLOADER_SIZE)

        actual_size = len(data)
        if actual_size < BOOTLOADER_SIZE:
            print(f"[warning] source file is smaller than expected. read only {actual_size} bytes.")

        with open(EXTRACT_DST, "wb") as f_out:
            f_out.write(data)

        print(f"[success] bootloader extracted and saved to: {EXTRACT_DST}")
    except Exception as e:
        print(f"[error] failed to extract bootloader: {e}")
        sys.exit(1)

def patch_firmware(boot_path, fw_path, out_path):
    if not os.path.exists(boot_path):
        print(f"[error] ota_boot not found at {boot_path}")
        sys.exit(1)
    if not os.path.exists(fw_path):
        print(f"[error] firmware not found at {fw_path}")
        sys.exit(1)

    try:
        with open(boot_path, "rb") as f:
            bootloader = f.read()

        with open(fw_path, "rb") as f:
            firmware = f.read()

        if len(firmware) < 8:
            print("[error] firmware binary is too small to patch!")
            sys.exit(1)

        # Apply trimming and concatenation
        fw_trimmed = firmware[4:-4]
        final_binary = bootloader + fw_trimmed

        with open(out_path, "wb") as f:
            f.write(final_binary)

        print(f"[success] patched and saved to: {out_path}")
    except Exception as e:
        print(f"[error] failed to patch firmware: {e}")
        sys.exit(1)

if __name__ == "__main__":
    # Validate command line arguments first to prevent unneeded file operations
    if len(sys.argv) < 4:
        print("usage: python soundcore_patcher.py <bootloader.bin> <firmware.bin> <output.bin>")
        print(f"example: python soundcore_patcher.py {EXTRACT_DST} openqore_test.bin firmware_patched.bin")
        sys.exit(1)

    # 1. Extract the bootloader from firmware.bin in the current folder
    extract_bootloader()
    print("-" * 50)

    # 2. Run the patching logic using the extracted bootloader and arguments
    patch_firmware(sys.argv[1], sys.argv[2], sys.argv[3])