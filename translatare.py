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

except Exception as e:
    print(f"Eroare la conectare: {e}")
    exit()

while True:
    if arduino.in_waiting > 0:
        data = arduino.readline().decode('utf-8').strip()
        
        if data:
            try:
                x_str, z_str, btn_str = data.split(',')
                x_val = int(x_str)
                z_val = int(z_str)
                btn_val = int(btn_str)

                prag = 15
                viteza = 1

                move_x = 0
                move_y = 0

                if x_val > latime_ecran: move_x = int((x_val - latime_ecran) * viteza)
                elif x_val < 0: move_x = int((x_val - 0) * viteza)

                if z_val > inaltime_ecran: move_y = int((z_val - inaltime_ecran) * viteza)
                elif z_val < 0: move_y = int((z_val - 0) * viteza)

                if move_x != 0 or move_y != 0:
                    pyautogui.move(move_x, move_y)

                if btn_val == 0:
                    pyautogui.click()
                    time.sleep(0.3)

            except ValueError:
                pass