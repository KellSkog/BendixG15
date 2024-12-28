'''Created by Anthropic Claude'''
import serial
import sys
import threading
import time
from pynput import keyboard
import argparse

def read_serial(ser):
    """Continuously read and print data from serial port"""
    while True:
        if ser.in_waiting:
            try:
                char = ser.read().decode('utf-8')
                print(char, end='', flush=True)
            except UnicodeDecodeError:
                print('?', end='', flush=True)
            except serial.SerialException:
                print("\nSerial port disconnected")
                break

def on_press(key, ser):
    """Handle keyboard press and send to serial port"""
    try:
        # For normal characters
        char = key.char
    except AttributeError:
        # Special keys
        if key == keyboard.Key.enter:
            char = '\r'
        elif key == keyboard.Key.space:
            char = ' '
        else:
            return  # Ignore other special keys
    
    try:
        ser.write(char.encode('utf-8'))
    except serial.SerialException:
        print("\nSerial port disconnected")
        sys.exit(1)

def main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='Serial port communication tool')
    parser.add_argument('port', help='Serial port to use (e.g., /dev/ttyUSB0 or COM1)')
    parser.add_argument('--baud', type=int, default=115200, help='Baud rate (default: 115200)')
    args = parser.parse_args()

    try:
        # Initialize serial port
        ser = serial.Serial(
            port=args.port,
            baudrate=args.baud,
            timeout=0.1
        )
        print(f"Connected to {args.port} at {args.baud} baud")
        
        # Start the serial reading thread
        read_thread = threading.Thread(target=read_serial, args=(ser,), daemon=True)
        read_thread.start()

        # Setup keyboard listener
        with keyboard.Listener(on_press=lambda key: on_press(key, ser)) as listener:
            listener.join()

    except serial.SerialException as e:
        print(f"Error: Could not open serial port {args.port}: {str(e)}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\nExiting...")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()

if __name__ == "__main__":
    main()