import serial # Requires the 'pySerial' library
import threading
import keyboard  # Requires the 'keyboard' library
import time

# Configure the serial port
ser = serial.Serial('COM12', 115200)  # Replace 'COM12' with your Pico's serial port
time.sleep(2)  # Wait for the connection to establish

file_transfer_mode = False

def read_from_pico():
    while True:
        if ser.in_waiting > 0:
            data = ser.read(ser.in_waiting).decode('utf-8')
            print(data, end='')

def send_file_to_pico(file_path):
    with open(file_path, 'r') as file:
        data = file.read()
        ser.write(data.encode())

def send_text_to_pico(text):
    ser.write(text.encode())

def on_key_event(event):
    global file_transfer_mode
    if event.name == 'f1':
        file_transfer_mode = not file_transfer_mode
        print("\nFile transfer mode:", "ON" if file_transfer_mode else "OFF")
    elif file_transfer_mode and event.name == 'enter':
        file_path = input("Enter file path: ")
        send_file_to_pico(file_path)
    elif not file_transfer_mode:
        send_text_to_pico(event.name)

# Start a thread to read from the Pico
thread = threading.Thread(target=read_from_pico)
thread.daemon = True
thread.start()

# Register the key event handler
keyboard.on_press(on_key_event)

print("Virtual terminal started. Press F1 to enter/exit file transfer mode. Type 'exit' to quit.")
while True:
    user_input = input()
    if user_input.lower() == 'exit':
        break

ser.close()
