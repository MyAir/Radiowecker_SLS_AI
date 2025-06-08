How to update the UI library:
1. Export the UI to a folder
2. Copy the folder to the lib/ui/src folder of the project  
3. Change /lib/src/ui.h: replace 
    #include "lvgl/lvgl.h"
with
    #include <lvgl.h>
    
