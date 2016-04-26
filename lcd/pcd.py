from ctypes import cdll
lib = cdll.LoadLibrary('/usr/share/emonPiLCD/PCD8544.so')

class lcd(object):
    def __init__(self):
        self.obj = lib.NewLCD()

    def lcd_display_string(self,text,line):
        lib.lcd_display_string(self.obj,text,line)
    def lcd_display_string_big(self,text,line):
        lib.lcd_display_string_big(self.obj,text,line)
    def lcd_display_line(self,x0,y0,x1,y1):
        lib.lcd_display_string(self.obj,x0,y0,x1,y1)        
    def backlight(self,brightness):
    	lib.backlight(self.obj,brightness)
