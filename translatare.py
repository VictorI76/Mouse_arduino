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

while True:
    if arduino.in_waiting > 0:
        data = arduino.readline().decode('utf-8').strip()
        
        if data:
            try:
                x_str, y_str, btn_str = data.split(',')
                x_val = int(x_str)
                y_val = int(y_str)
                btn_val = int(btn_str)

                viteza = 1

                move_x = 0
                move_y = 0

                if x_val > latime_ecran: 
                    move_x = latime_ecran
                elif x_val < 0: 
                    move_x = 0

                if y_val > inaltime_ecran: 
                    move_y = inaltime_ecran
                elif y_val < 0: 
                    move_y = 0

                if move_x >= 0 or move_y >= 0:
                    pyautogui.move(move_x, move_y)

                if btn_val == 1:
                    pyautogui.click()
                    time.sleep(0.3)
                    btn_val = 0

            except ValueError:
                pass