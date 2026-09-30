import serial
import pyautogui
import time

PORT = 'COM9'
BAUD_RATE = 9600

try:
    arduino = serial.Serial(PORT, BAUD_RATE, timeout=0.1)
    print(f"Connect on {PORT}. Waiting for arduino")
    
    time.sleep(2)
    
    width_screen, hight_screen = pyautogui.size()
    print(f"Screen rezolution {width_screen}x{hight_screen}")
    
    msg_rezolution = f"W{width_screen},H{hight_screen}\n"
    
    arduino.write(msg_rezolution.encode('utf-8'))
    print("The sizes have been sent.")

    message = arduino.readline()
    print(message.decode('utf-8').strip())

except Exception as e:
    print(f"Eroare la conectare: {e}")
    exit()

btn_pressed = False
pyautogui.PAUSE = 0

while True:
    if arduino.in_waiting > 0:
        data = arduino.readline().decode('utf-8').strip()
        
        if data:
            try:
                x_str, y_str, btn_str = data.split(',')
                x_val = int(x_str)
                y_val = int(y_str)
                btn_val = int(btn_str)

                x_val = max(0, min(x_val, width_screen))
                y_val = max(0, min(y_val, hight_screen))

                pyautogui.moveTo(x_val, y_val)

                print(f"X:{x_val}, Y:{y_val}")

                if btn_val == 1 and not btn_pressed:
                    pyautogui.click()
                    btn_pressed = True
                elif btn_val == 0:
                    btn_pressed = False

            except ValueError:
                pass