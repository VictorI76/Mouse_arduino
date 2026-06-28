import serial
import pyautogui
import time

# Setează portul tău COM (verifică în Arduino IDE la Tools -> Port)
PORT = 'COM3' 
BAUD_RATE = 9600

# Conectarea la Arduino
try:
    arduino = serial.Serial(PORT, BAUD_RATE)
    print(f"Conectat la Arduino pe {PORT}. Mișcă joystick-ul!")
except Exception as e:
    print(f"Eroare la conectare: {e}")
    exit()

while True:
    if arduino.in_waiting > 0:
        # Citim și decodăm linia primită
        data = arduino.readline().decode('utf-8').strip()
        
        if data:
            try:
                # Împărțim textul în variabile
                x_str, y_str, btn_str = data.split(',')
                x_val = int(x_str)
                y_val = int(y_str)
                btn_val = int(btn_str)

                move_x = 0
                move_y = 0
                viteza = 10 # Câți pixeli să se miște

                # Când joystick-ul este în centru, valoarea e undeva la 512
                # Creăm o zonă moartă (deadzone) între 400 și 600 ca să nu se miște singur
                if x_val > 600: move_x = viteza
                elif x_val < 400: move_x = -viteza

                if y_val > 600: move_y = viteza
                elif y_val < 400: move_y = -viteza

                # Mutăm cursorul doar dacă este cazul
                if move_x != 0 or move_y != 0:
                    # Mișcăm relativ față de poziția curentă
                    pyautogui.move(move_x, move_y)

                # Logica de click (dacă e 0, înseamnă că e apăsat datorită INPUT_PULLUP)
                if btn_val == 0:
                    pyautogui.click()
                    time.sleep(0.3) # Pauză scurtă pentru a evita click-uri multiple

            except ValueError:
                # Ignorăm erorile dacă Arduino a trimis date incomplete
                pass