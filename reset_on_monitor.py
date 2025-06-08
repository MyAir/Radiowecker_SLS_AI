Import("env")
import time
import os

# This will execute before the monitor tool is connected
def before_monitor_connected(source, target, env):
    print("\033[1;32mPerforming hardware reset of ESP32-S3 before monitor connects...\033[0m")
    # Get the current upload port
    port = env.subst("$UPLOAD_PORT")
    if not port:
        print("\033[1;31mERROR: No upload port defined!\033[0m")
        return
    
    from serial import Serial
    # Open serial port
    try:
        # Configure serial port
        ser = Serial(port, 115200, timeout=1)
        
        # ESP32-S3 reset sequence - toggle DTR/RTS lines with specific timing
        # First ensure both lines are inactive
        ser.setDTR(False)
        ser.setRTS(False)
        time.sleep(0.1)
        
        # Now perform EN pin reset (DTR controls EN pin via inverter)
        ser.setDTR(True)   # EN pin goes LOW (module in reset)
        time.sleep(0.1)
        ser.setDTR(False)  # EN pin goes HIGH (module boots)
        time.sleep(0.1)
        
        # Close the connection to allow monitor to take over
        ser.close()
        print("\033[1;32mDevice reset completed successfully\033[0m")
        
        # Small delay to ensure device has time to start booting before monitor connects
        time.sleep(0.5)
    except Exception as e:
        print(f"\033[1;31mFailed to reset device: {e}\033[0m")

# Register the callback to run before monitor
env.AddPreAction("monitor", before_monitor_connected)
