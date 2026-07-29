import serial
import pyautogui
import time

PORT = 'COM3'
BAUD_RATE = 9600

try:
    arduino = serial.Serial(PORT, BAUD_RATE, timeout=0.1)
    print(f"Conectat pe {PORT}. Așteptăm inițializarea Arduino...")
    
    time.sleep(2)
    
    latime_ecran, inaltime_ecran = pyautogui.size()
    print(f"Rezoluție ecran detectată: {latime_ecran}x{inaltime_ecran}")
    
    mesaj_rezolutie = f"W{latime_ecran},H{inaltime_ecran}\n"
    
    arduino.write(mesaj_rezolutie.encode('utf-8'))
    print("Marginile au fost trimise către Arduino. Pornește citirea senzorului!")

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

                x_val = max(0, min(x_val, latime_ecran))
                y_val = max(0, min(y_val, inaltime_ecran))

                pyautogui.moveTo(x_val, y_val)

                if btn_val == 1 and not btn_pressed:
                    pyautogui.click()
                    btn_pressed = True
                elif btn_val == 0:
                    btn_pressed = False

            except ValueError:
                pass